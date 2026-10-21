#include <stdio.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(void)
{
    lua_State* state = luaL_newstate();
    if (!state)
    {
        fputs("failed to allocate a Lua state\n", stderr);
        return 1;
    }

    luaL_openlibs(state);

    if (luaL_dostring(state,
        "local sum = 0 "
        "for i = 1, 10 do sum = sum + i end "
        "assert(sum == 55) "
        "assert(type(string.format) == 'function') "
        "return sum") != 0)
    {
        fprintf(stderr, "Lua smoke script failed: %s\n", lua_tostring(state, -1));
        lua_close(state);
        return 2;
    }

    if (!lua_isnumber(state, -1) || lua_tointeger(state, -1) != 55)
    {
        fputs("Lua smoke script returned an unexpected value\n", stderr);
        lua_close(state);
        return 3;
    }

    printf("Eluna Lua runtime smoke test passed (%s, configured as %s)\n",
           LUA_VERSION, ELUNA_EXPECTED_LUA_VERSION);
    lua_close(state);
    return 0;
}
