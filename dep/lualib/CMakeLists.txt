set(LUA_VERSION "${ELUNA_LUA_VERSION}")

if(NOT LUA_VERSION MATCHES "^lua(51|52|53|54)$")
  message(FATAL_ERROR "ELUNA_LUA_VERSION must be one of lua51, lua52, lua53, or lua54")
endif()

message(STATUS "Eluna Lua runtime     : ${LUA_VERSION}")

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

add_subdirectory(lua)

if(BUILD_ELUNA_TESTS)
  add_executable(eluna_lua_smoke tests/lua_smoke.c)
  target_link_libraries(eluna_lua_smoke PRIVATE lualib)
  target_compile_definitions(eluna_lua_smoke PRIVATE
    ELUNA_EXPECTED_LUA_VERSION="${ELUNA_LUA_VERSION}"
  )
  set_target_properties(eluna_lua_smoke PROPERTIES FOLDER "Tests")
  add_test(NAME eluna.lua-runtime COMMAND eluna_lua_smoke)

  # LuaVal from the patched Eluna copy (dep/eluna-patches 0012: cycles and deep nesting refused).
  add_executable(eluna_luaval_smoke
    tests/luaval_smoke.cpp
    ${ELUNA_SOURCE_DIR}/LuaValue.cpp
    ${ELUNA_SOURCE_DIR}/ElunaCompat.cpp
  )
  target_include_directories(eluna_luaval_smoke PRIVATE ${ELUNA_SOURCE_DIR})
  target_compile_definitions(eluna_luaval_smoke PRIVATE ELUNA_VMANGOS)
  target_link_libraries(eluna_luaval_smoke PRIVATE lualib)
  set_target_properties(eluna_luaval_smoke PROPERTIES FOLDER "Tests" LINKER_LANGUAGE CXX)
  add_test(NAME eluna.luaval COMMAND eluna_luaval_smoke)
endif()
