GetModuleSourceList(MODULES_MODULE_LIST)

set(MODULES_PUBLIC_INCLUDES)
set(MODULES_STATIC_SOURCES)
set(MODULES_STATIC_MODULES)
set(MODULES_DYNAMIC_MODULES)
set(MODULES_DYNAMIC_PROJECTS)
set(MODULES_ENABLED_MODULES)

if(MODULES STREQUAL "dynamic")
  set(MODULES_DEFAULT_LINKAGE "dynamic")
elseif(MODULES STREQUAL "static")
  set(MODULES_DEFAULT_LINKAGE "static")
else()
  set(MODULES_DEFAULT_LINKAGE "disabled")
endif()

if(WIN32)
  set(MODULES_BUILD_RUNTIME_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/modules")
else()
  set(MODULES_BUILD_RUNTIME_DIR "${CMAKE_BINARY_DIR}/modules")
endif()

set(MODULES_INSTALL_RUNTIME_DIR "${LIBS_DIR}/modules")

set(MODULES_COMMON_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src/scripts
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/framework/Network
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/src/game/AI
  ${CMAKE_SOURCE_DIR}/src/game/Anticheat
  ${CMAKE_SOURCE_DIR}/src/game/AuctionHouse
  ${CMAKE_SOURCE_DIR}/src/game/Battlegrounds
  ${CMAKE_SOURCE_DIR}/src/game/Chat
  ${CMAKE_SOURCE_DIR}/src/game/Commands
  ${CMAKE_SOURCE_DIR}/src/game/Database
  ${CMAKE_SOURCE_DIR}/src/game/Group
  ${CMAKE_SOURCE_DIR}/src/game/Guild
  ${CMAKE_SOURCE_DIR}/src/game/Handlers
  ${CMAKE_SOURCE_DIR}/src/game/LFG
  ${CMAKE_SOURCE_DIR}/src/game/Mail
  ${CMAKE_SOURCE_DIR}/src/game/Maps
  ${CMAKE_SOURCE_DIR}/src/game/Maps/Pool
  ${CMAKE_SOURCE_DIR}/src/game/Movement
  ${CMAKE_SOURCE_DIR}/src/game/Movement/spline
  ${CMAKE_SOURCE_DIR}/src/game/Objects
  ${CMAKE_SOURCE_DIR}/src/game/OutdoorPvP
  ${CMAKE_SOURCE_DIR}/src/game/Protocol
  ${CMAKE_SOURCE_DIR}/src/game/Spells
  ${CMAKE_SOURCE_DIR}/src/game/Threat
  ${CMAKE_SOURCE_DIR}/src/game/Transports
  ${CMAKE_SOURCE_DIR}/src/game/vmap
  ${CMAKE_SOURCE_DIR}/src/game/LFT
  ${CMAKE_SOURCE_DIR}/src/game/MapNodes
  ${CMAKE_SOURCE_DIR}/src/game/PacketBroadcast
  ${CMAKE_SOURCE_DIR}/src/game/Logging
  ${CMAKE_SOURCE_DIR}/src/game/Shop
  ${CMAKE_SOURCE_DIR}/src/game/TWDebuff
  ${CMAKE_SOURCE_DIR}/src/game/GuildBank
  ${CMAKE_SOURCE_DIR}/src/game/HttpApi
  ${CMAKE_SOURCE_DIR}/src/game/DiscordBot
  ${CMAKE_SOURCE_DIR}/src/game/Analysis
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src/shared
  # Was missing from this list entirely (modules #including <openssl/x.h> had no way to
  # resolve it), and needs to come first - see the OpenSSL/ACE/TBB ordering note in
  # src/mangosd's CMakeLists.txt.
  ${OPENSSL_INCLUDE_DIR}
  ${TBB_INCLUDE_DIRS}
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
)

function(GetModuleProjectName module variable)
  string(TOLOWER "mod_${module}" MODULE_PROJECT_NAME)
  string(REGEX REPLACE "[^a-z0-9_]" "_" MODULE_PROJECT_NAME "${MODULE_PROJECT_NAME}")
  set(${variable} "${MODULE_PROJECT_NAME}" PARENT_SCOPE)
endfunction()

function(GetModuleLoaderFunction module variable)
  string(REGEX REPLACE "[^A-Za-z0-9_]" "_" MODULE_FUNCTION_NAME "${module}")
  set(${variable} "Add${MODULE_FUNCTION_NAME}Scripts" PARENT_SCOPE)
endfunction()

function(GetModuleScopedProperty module_variable property_suffix variable)
  TortoiseGetGlobalProperty("TW_${module_variable}_${property_suffix}" PROPERTY_VALUE)
  set(${variable} ${PROPERTY_VALUE} PARENT_SCOPE)
endfunction()

function(ConfigureModuleLoader loader_name loader_out is_dynamic)
  set(TORTOISE_IS_DYNAMIC_MODULE_LOADER ${is_dynamic})
  set(TORTOISE_CURRENT_MODULE_PROJECT "${loader_name}")
  set(TORTOISE_MODULES_BUILD_DIRECTIVE "${CMAKE_BUILD_TYPE}")
  unset(TORTOISE_MODULES_FORWARD_DECL)
  unset(TORTOISE_MODULES_INVOKE)
  unset(TORTOISE_MODULES_EXTRA_INCLUDES)
  unset(TORTOISE_MODULES_EXTRA_INVOKE)

  unset(TW_ADD_SCRIPTS_INCLUDE)
  unset(TW_ADD_SCRIPTS_LIST)
  foreach(MODULE_NAME ${ARGN})
    ModuleNameToVariable("${MODULE_NAME}" MODULE_VARIABLE)
    GetModuleScopedProperty("${MODULE_VARIABLE}" "ADD_SCRIPTS_INCLUDE" MODULE_SCRIPT_INCLUDES)
    GetModuleScopedProperty("${MODULE_VARIABLE}" "ADD_SCRIPTS_LIST" MODULE_SCRIPT_LOADERS)
    list(APPEND TW_ADD_SCRIPTS_INCLUDE ${MODULE_SCRIPT_INCLUDES})
    list(APPEND TW_ADD_SCRIPTS_LIST ${MODULE_SCRIPT_LOADERS})
  endforeach()
  list(REMOVE_DUPLICATES TW_ADD_SCRIPTS_INCLUDE)

  foreach(TW_SCRIPT_INCLUDE ${TW_ADD_SCRIPTS_INCLUDE})
    set(TORTOISE_MODULES_EXTRA_INCLUDES "${TORTOISE_MODULES_EXTRA_INCLUDES}#include \"${TW_SCRIPT_INCLUDE}\"\n")
  endforeach()

  foreach(TW_SCRIPT_LOADER ${TW_ADD_SCRIPTS_LIST})
    string(REGEX REPLACE ";$" "" TW_SCRIPT_LOADER_CALL "${TW_SCRIPT_LOADER}")
    set(TORTOISE_MODULES_EXTRA_INVOKE "${TORTOISE_MODULES_EXTRA_INVOKE}    ${TW_SCRIPT_LOADER_CALL};\n")
  endforeach()

  foreach(MODULE_NAME ${ARGN})
    GetModuleLoaderFunction("${MODULE_NAME}" MODULE_LOADER_FUNCTION)
    set(TORTOISE_MODULES_FORWARD_DECL "${TORTOISE_MODULES_FORWARD_DECL}void ${MODULE_LOADER_FUNCTION}();\n")
    set(TORTOISE_MODULES_INVOKE "${TORTOISE_MODULES_INVOKE}    ${MODULE_LOADER_FUNCTION}();\n")
  endforeach()

  set(GENERATED_LOADER "${CMAKE_CURRENT_BINARY_DIR}/generated/${loader_name}/ModulesLoader.cpp")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ModulesLoader.cpp.in.cmake" "${GENERATED_LOADER}")
  set(${loader_out} "${GENERATED_LOADER}" PARENT_SCOPE)
endfunction()

foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
  GetPathToModuleSource("${SOURCE_MODULE}" MODULE_SOURCE_PATH)
  ModuleNameToVariable("${SOURCE_MODULE}" MODULE_VARIABLE)
  GetModuleProjectName("${SOURCE_MODULE}" MODULE_PROJECT_NAME)

  if("${${MODULE_VARIABLE}}" STREQUAL "default")
    set(MODULE_LINKAGE "${MODULES_DEFAULT_LINKAGE}")
  else()
    set(MODULE_LINKAGE "${${MODULE_VARIABLE}}")
  endif()

  if(NOT MODULE_LINKAGE STREQUAL "disabled")
    set(TORTOISE_MODULE_CMAKE_PHASE DISCOVERY)
    set(TORTOISE_CURRENT_MODULE "${SOURCE_MODULE}")
    set(TORTOISE_CURRENT_MODULE_VARIABLE "${MODULE_VARIABLE}")
    set(TORTOISE_CURRENT_MODULE_LINKAGE "${MODULE_LINKAGE}")
    include("${CMAKE_SOURCE_DIR}/modules/${SOURCE_MODULE}/${SOURCE_MODULE}.cmake" OPTIONAL)
    unset(TORTOISE_CURRENT_MODULE_LINKAGE)
    unset(TORTOISE_CURRENT_MODULE_VARIABLE)
    unset(TORTOISE_CURRENT_MODULE)
    unset(TORTOISE_MODULE_CMAKE_PHASE)

    list(APPEND MODULES_ENABLED_MODULES "${SOURCE_MODULE}")

    GetModuleScopedProperty("${MODULE_VARIABLE}" "SCRIPTS_SOURCES" MODULE_EXTRA_SCRIPT_SOURCES)
    if(MODULE_EXTRA_SCRIPT_SOURCES)
      list(REMOVE_DUPLICATES MODULE_EXTRA_SCRIPT_SOURCES)
    endif()
    GetModuleScopedProperty("${MODULE_VARIABLE}" "SCRIPTS_INCLUDE_DIRS" MODULE_EXTRA_SCRIPT_INCLUDE_DIRS)
    if(MODULE_EXTRA_SCRIPT_INCLUDE_DIRS)
      list(REMOVE_DUPLICATES MODULE_EXTRA_SCRIPT_INCLUDE_DIRS)
    endif()
  endif()

  if(NOT MODULE_LINKAGE STREQUAL "disabled")
    file(GLOB MODULE_CONFIG_DIST_FILES CONFIGURE_DEPENDS
      "${CMAKE_SOURCE_DIR}/modules/${SOURCE_MODULE}/conf/*.conf.dist")

    foreach(MODULE_CONFIG_DIST_FILE ${MODULE_CONFIG_DIST_FILES})
      CopyModuleConfig("${MODULE_CONFIG_DIST_FILE}")
    endforeach()
  endif()

  if(MODULE_LINKAGE STREQUAL "static")
    CollectModuleSourceFiles("${MODULE_SOURCE_PATH}" MODULE_SOURCE_FILES)
    CollectModuleIncludeDirectories("${MODULE_SOURCE_PATH}" MODULE_INCLUDE_DIRS)
    list(APPEND MODULES_STATIC_SOURCES ${MODULE_SOURCE_FILES})
    list(APPEND MODULES_STATIC_SOURCES ${MODULE_EXTRA_SCRIPT_SOURCES})
    list(APPEND MODULES_PUBLIC_INCLUDES ${MODULE_INCLUDE_DIRS})
    list(APPEND MODULES_PUBLIC_INCLUDES ${MODULE_EXTRA_SCRIPT_INCLUDE_DIRS})
    list(APPEND MODULES_STATIC_MODULES "${SOURCE_MODULE}")
  elseif(MODULE_LINKAGE STREQUAL "dynamic")
    CollectModuleSourceFiles("${MODULE_SOURCE_PATH}" MODULE_SOURCE_FILES)
    CollectModuleIncludeDirectories("${MODULE_SOURCE_PATH}" MODULE_INCLUDE_DIRS)
    ConfigureModuleLoader("${SOURCE_MODULE}" MODULE_DYNAMIC_LOADER ON "${SOURCE_MODULE}")

    if(MSVC)
      # The module's code is compiled apart from its DLL: mangosd reads what these objects ask
      # of the core before it links, and exports exactly that (cmake/ModuleExports.cmake). The
      # DLL then links against mangosd itself -- one core, one set of singletons -- where it
      # used to link game, shared and framework statically and hold a private copy of each.
      # MSVC only: /FI and dumpbin are its own. Other Windows toolchains keep the old path.
      add_library(${MODULE_PROJECT_NAME}_objects OBJECT
        ${MODULE_SOURCE_FILES}
        ${MODULE_EXTRA_SCRIPT_SOURCES}
        ${MODULE_DYNAMIC_LOADER})
      target_include_directories(${MODULE_PROJECT_NAME}_objects
        PUBLIC
          ${MODULES_COMMON_INCLUDES}
          ${MODULE_INCLUDE_DIRS}
          ${MODULE_EXTRA_SCRIPT_INCLUDE_DIRS})
      target_compile_options(${MODULE_PROJECT_NAME}_objects
        PRIVATE "/FI${CMAKE_CURRENT_SOURCE_DIR}/ModuleImports.h")
      # The core's data the module reads -- the script registries -- comes dllimport'ed from
      # mangosd (TW_CORE_DATA, ScriptMgr.h), not as the module's own copy.
      target_compile_definitions(${MODULE_PROJECT_NAME}_objects PRIVATE TORTOISE_MSVC_DYNAMIC_MODULE)
      # The core's usage requirements -- its definitions and includes -- and nothing linked: an
      # object library only compiles.
      target_link_libraries(${MODULE_PROJECT_NAME}_objects PRIVATE game shared framework)
      add_library(${MODULE_PROJECT_NAME} SHARED
        $<TARGET_OBJECTS:${MODULE_PROJECT_NAME}_objects>)
      set_target_properties(mangosd PROPERTIES ENABLE_EXPORTS ON)
      # mangosd's import library alone -- not the target, whose own static libraries CMake would
      # pass on and link into the DLL a second time.
      add_dependencies(${MODULE_PROJECT_NAME} mangosd)
      target_link_libraries(${MODULE_PROJECT_NAME}
        PRIVATE
          $<TARGET_LINKER_FILE:mangosd>
          ${ACE_LIBRARIES})
      list(APPEND MODULES_WINDOWS_OBJECT_TARGETS ${MODULE_PROJECT_NAME}_objects)
    else()
      add_library(${MODULE_PROJECT_NAME} SHARED
        ${MODULE_SOURCE_FILES}
        ${MODULE_EXTRA_SCRIPT_SOURCES}
        ${MODULE_DYNAMIC_LOADER})

      target_include_directories(${MODULE_PROJECT_NAME}
        PUBLIC
          ${MODULES_COMMON_INCLUDES}
          ${MODULE_INCLUDE_DIRS}
          ${MODULE_EXTRA_SCRIPT_INCLUDE_DIRS})

      if(WIN32)
        target_link_libraries(${MODULE_PROJECT_NAME}
          PUBLIC
            game
            shared
            framework)
      else()
        target_link_libraries(${MODULE_PROJECT_NAME}
          PUBLIC
            fmt)
      endif()
    endif()

    set_target_properties(${MODULE_PROJECT_NAME}
      PROPERTIES
        FOLDER "modules"
        OUTPUT_NAME "${SOURCE_MODULE}"
        LIBRARY_OUTPUT_DIRECTORY "${MODULES_BUILD_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${MODULES_BUILD_RUNTIME_DIR}")

    install(TARGETS ${MODULE_PROJECT_NAME}
      LIBRARY DESTINATION "${MODULES_INSTALL_RUNTIME_DIR}"
      RUNTIME DESTINATION "${MODULES_INSTALL_RUNTIME_DIR}")

    list(APPEND MODULES_DYNAMIC_MODULES ${SOURCE_MODULE})
    list(APPEND MODULES_DYNAMIC_PROJECTS ${MODULE_PROJECT_NAME})
  elseif(NOT MODULE_LINKAGE STREQUAL "disabled")
    message(FATAL_ERROR "Unknown linkage \"${MODULE_LINKAGE}\" for module ${SOURCE_MODULE}.")
  endif()
endforeach()

# **mangosd exports what the dynamic modules ask of it** (MSVC; Linux has -rdynamic): read
# from their objects just before mangosd links, into a .def it links with, and an import library
# the modules link against.
if(MSVC AND MODULES_WINDOWS_OBJECT_TARGETS)
  get_filename_component(MODULES_LINKER_DIR "${CMAKE_LINKER}" DIRECTORY)
  set(MODULES_EXPORTS_DEF "${CMAKE_BINARY_DIR}/mangosd_modules.def")
  # The core a module builds on: the game, its shared and framework layers, and the scripts'
  # own base (ScriptedInstance, ScriptedAI) when the core is built with them.
  set(MODULES_EXPORT_LIB_TARGETS game shared framework)
  if(TARGET scripts)
    list(APPEND MODULES_EXPORT_LIB_TARGETS scripts)
  endif()
  set(MODULES_EXPORT_LIBS)
  foreach(LIB_TARGET ${MODULES_EXPORT_LIB_TARGETS})
    list(APPEND MODULES_EXPORT_LIBS "$<TARGET_FILE:${LIB_TARGET}>")
  endforeach()
  set(MODULES_EXPORT_OBJECTS)
  foreach(OBJECT_TARGET ${MODULES_WINDOWS_OBJECT_TARGETS})
    list(APPEND MODULES_EXPORT_OBJECTS "$<TARGET_OBJECTS:${OBJECT_TARGET}>")
  endforeach()
  # A command here and a target mangosd waits for: a custom command can only hang on a target
  # made in its own directory, and mangosd is made in src/mangosd, before the modules are known.
  # Each list goes over as one "|"-joined argument; the script splits it again.
  add_custom_command(OUTPUT "${MODULES_EXPORTS_DEF}"
    COMMAND "${CMAKE_COMMAND}"
      "-DDUMPBIN=${MODULES_LINKER_DIR}/dumpbin.exe"
      "-DOBJECTS=$<JOIN:${MODULES_EXPORT_OBJECTS},|>"
      "-DLIBS=$<JOIN:${MODULES_EXPORT_LIBS},|>"
      "-DOUT=${MODULES_EXPORTS_DEF}"
      -P "${CMAKE_SOURCE_DIR}/cmake/ModuleExports.cmake"
    DEPENDS
      ${MODULES_EXPORT_OBJECTS} ${MODULES_EXPORT_LIBS}
      "${CMAKE_SOURCE_DIR}/cmake/ModuleExports.cmake"
    COMMENT "mangosd: exporting what the dynamic modules need"
    VERBATIM)
  add_custom_target(mangosd_module_exports DEPENDS "${MODULES_EXPORTS_DEF}")
  add_dependencies(mangosd mangosd_module_exports)
  set_property(TARGET mangosd APPEND PROPERTY LINK_OPTIONS "/DEF:${MODULES_EXPORTS_DEF}")
  set_property(TARGET mangosd APPEND PROPERTY LINK_DEPENDS "${MODULES_EXPORTS_DEF}")
endif()

ConfigureModuleLoader("static" MODULE_STATIC_LOADER OFF ${MODULES_STATIC_MODULES})

add_library(modules STATIC
  ModulesScriptLoader.h
  ${MODULE_STATIC_LOADER}
  ${MODULES_STATIC_SOURCES})

target_include_directories(modules
  PUBLIC
    ${MODULES_COMMON_INCLUDES}
    ${MODULES_PUBLIC_INCLUDES})

target_link_libraries(modules
  PUBLIC
    game)

set_target_properties(modules PROPERTIES FOLDER "modules")

ProcessModuleConfigCopies(modules)

set(TORTOISE_MODULE_CMAKE_PHASE POST_TARGETS)
foreach(SOURCE_MODULE ${MODULES_ENABLED_MODULES})
  ModuleNameToVariable("${SOURCE_MODULE}" MODULE_VARIABLE)
  if("${${MODULE_VARIABLE}}" STREQUAL "default")
    set(TORTOISE_CURRENT_MODULE_LINKAGE "${MODULES_DEFAULT_LINKAGE}")
  else()
    set(TORTOISE_CURRENT_MODULE_LINKAGE "${${MODULE_VARIABLE}}")
  endif()
  set(TORTOISE_CURRENT_MODULE "${SOURCE_MODULE}")
  set(TORTOISE_CURRENT_MODULE_VARIABLE "${MODULE_VARIABLE}")
  include("${CMAKE_SOURCE_DIR}/modules/${SOURCE_MODULE}/${SOURCE_MODULE}.cmake" OPTIONAL)
  unset(TORTOISE_CURRENT_MODULE_VARIABLE)
  unset(TORTOISE_CURRENT_MODULE)
  unset(TORTOISE_CURRENT_MODULE_LINKAGE)
endforeach()
unset(TORTOISE_MODULE_CMAKE_PHASE)

set(WORLDSERVER_DYNAMIC_MODULES_DEPENDENCIES ${MODULES_DYNAMIC_PROJECTS} PARENT_SCOPE)
set(WORLDSERVER_DYNAMIC_MODULES ${MODULES_DYNAMIC_MODULES} PARENT_SCOPE)
set(WORLDSERVER_ENABLED_MODULES ${MODULES_ENABLED_MODULES} PARENT_SCOPE)
set(MODULES_BUILD_RUNTIME_DIR "${MODULES_BUILD_RUNTIME_DIR}" PARENT_SCOPE)
