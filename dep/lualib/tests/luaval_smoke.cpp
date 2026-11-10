// LuaVal (the value of WorldObject:Data() / Map:Data()) under the conversions that crashed mangosd
// before the patch-0012 fix: a table that holds itself, nesting at and past the 64-level limit, an
// unsupported value, and those inside a coroutine. Built from the patched Eluna copy (ELUNA_SOURCE_DIR)
// with ELUNA_VMANGOS, run by ctest as eluna.luaval. Before the fix the self-reference case overflowed the
// Lua stack of the C call (heap corruption, SIGSEGV or "stack smashing detected").
#include <cstdio>

#include "ElunaCompat.h"
#include "LuaValue.h"

extern "C"
{
#include "lualib.h"
}

static char const* const SCRIPT = R"LUA(
local function check(cond, what) if not cond then error("FAIL " .. what, 2) end end
local function refused(fn, needle, ...)
    local ok, err = pcall(fn, ...)
    check(not ok, needle .. ": accepted")
    check(tostring(err):find(needle, 1, true), needle .. ": wrong error " .. tostring(err))
end
local function chain(n)  -- n plain tables, each in the next one's "c"
    local t = { leaf = true }
    for i = 2, n do t = { c = t } end
    return t
end

local d = LuaVal.New({})
for round = 1, 200 do  -- many refusals: a leak or a stack overrun would show up
    local selfRef = {}
    selfRef.self = selfRef
    refused(d.Set, "contains itself", d, "selfref", selfRef)
    check(d:Get("selfref") == nil, "selfref stored")
    local a, b = {}, {}
    a.b = b; b.a = a
    refused(d.Set, "contains itself", d, "ab", a)
    refused(LuaVal.New, "contains itself", selfRef)
    local k = {}; k[k] = 1  -- a cycle through a key
    refused(d.Set, "contains itself", d, "k", k)
    refused(d.Set, "deeper than 64", d, "deep", chain(65))
    refused(d.Set, "unsupported type", d, "f", function() end)
    refused(d.Set, "unsupported type", d, "deepf", { { { { function() end } } } })
    refused(d.Set, "NaN as key", d, 0/0, 1)
end

d:Set("deep", chain(64))  -- exactly at the limit: stored
local t = d:AsTable()
local depth, x = 1, t.deep
while x.c do x = x.c; depth = depth + 1 end
check(depth == 64 and x.leaf == true, "AsTable of 64 levels: " .. depth)

local shared = { v = 1 }
d:Set("dag", { a = shared, b = shared })  -- the same table twice side by side is no cycle
check(d:Get("dag", "a", "v") == 1 and d:Get("dag", "b", "v") == 1, "dag")

local inner = LuaVal.New(chain(64))
d:Set("lv", inner)  -- a LuaVal at the top counts from level 0: stored
refused(d.Set, "deeper than 64", d, "lv2", { wrap = inner })  -- one level more: refused

refused(d.Set, "deeper than 64", d, "me", d)  -- d holds 64 levels: inside itself it is one more

local e = LuaVal.New({ n = 1 })
e:Set("me", e)  -- value copy, not a cycle
check(e:Get("me"):Get("me") == nil and e:Get("me"):Get("n") == 1, "copy")

coroutine.wrap(function()
    local s = {}; s.s = s
    refused(d.Set, "contains itself", d, "co", s)
    refused(d.Set, "deeper than 64", d, "co2", chain(100))
    d:Set("co3", chain(64))
end)()
check(d:Get("co3") ~= nil, "coroutine store")
collectgarbage()
return "ok"
)LUA";

int main()
{
    lua_State* L = luaL_newstate();
    if (!L)
        return 1;
    luaL_openlibs(L);
    LuaVal::Register(L);
    if (luaL_dostring(L, SCRIPT) != 0)
    {
        std::fprintf(stderr, "LuaVal smoke failed: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 2;
    }
    lua_close(L);
    std::printf("LuaVal smoke passed: cycles and nesting past 64 levels refused with a Lua error\n");
    return 0;
}
