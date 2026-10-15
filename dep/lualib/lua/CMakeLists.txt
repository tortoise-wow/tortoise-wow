# Lua is fetched from lua.org with a pinned checksum and built as a static
# library so Eluna has the same runtime on every supported platform.
project(lua C)

include(FetchContent)

FetchContent_Declare(
  lua51
  URL https://www.lua.org/ftp/lua-5.1.5.tar.gz
  URL_HASH SHA256=2640fc56a795f29d28ef15e13c34a47e223960b0240e8cb0a82d9b0738695333
)
FetchContent_Declare(
  lua52
  URL https://www.lua.org/ftp/lua-5.2.4.tar.gz
  URL_HASH SHA256=b9e2e4aad6789b3b63a056d442f7b39f0ecfca3ae0f1fc0ae4e9614401b69f4b
)
FetchContent_Declare(
  lua53
  URL https://www.lua.org/ftp/lua-5.3.6.tar.gz
  URL_HASH SHA256=fc5fd69bb8736323f026672b1b7235da613d7177e72558893a0bdcd320466d60
)
FetchContent_Declare(
  lua54
  URL https://www.lua.org/ftp/lua-5.4.4.tar.gz
  URL_HASH SHA256=164c7849653b80ae67bec4b7473b884bf5cc8d2dca05653475ec2ed27b9ebf61
)

FetchContent_MakeAvailable(${LUA_VERSION})

set(LUA_SOURCE_DIR "${${LUA_VERSION}_SOURCE_DIR}/src")
file(GLOB LUA_HEADERS CONFIGURE_DEPENDS ${LUA_SOURCE_DIR}/*.h)
file(GLOB LUA_SOURCES CONFIGURE_DEPENDS ${LUA_SOURCE_DIR}/*.c)
list(REMOVE_ITEM LUA_SOURCES ${LUA_SOURCE_DIR}/lua.c ${LUA_SOURCE_DIR}/luac.c)

add_library(lualib STATIC ${LUA_HEADERS} ${LUA_SOURCES})
set_target_properties(lualib PROPERTIES
  LINKER_LANGUAGE C
  POSITION_INDEPENDENT_CODE ON
  FOLDER "Dependencies"
)
target_include_directories(lualib PUBLIC ${LUA_SOURCE_DIR})

if(WIN32)
  target_compile_definitions(lualib PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
  target_compile_definitions(lualib PUBLIC LUA_USE_MACOSX)
  target_compile_options(lualib PRIVATE -Wno-deprecated-declarations -Wno-empty-body)
else()
  target_compile_definitions(lualib PUBLIC LUA_USE_LINUX)
  target_link_libraries(lualib PUBLIC ${CMAKE_DL_LIBS} m)
endif()

# Eluna targets Lua 5.2 by default but keeps compatibility shims for the other
# supported runtimes. LUA_COMPAT_MODULE restores the legacy module() API.
target_compile_definitions(lualib PUBLIC LUA_COMPAT_MODULE)
