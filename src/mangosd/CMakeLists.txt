# Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
# Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

set(EXECUTABLE_NAME mangosd)

# SOAP remote-command interface. The gsoap C++ runtime is vendored as a Linux
# static lib (dep/lib/gsoap/libgsoap++.a), so SOAP does not build on Windows.
# Default: ON for Linux, OFF for Windows. Override with -DENABLE_SOAP=ON/OFF.
if(WIN32)
  set(_ENABLE_SOAP_DEFAULT OFF)
else()
  set(_ENABLE_SOAP_DEFAULT ON)
endif()
option(ENABLE_SOAP "Build the SOAP remote-command interface (vendored gsoap, Linux only)" ${_ENABLE_SOAP_DEFAULT})
set (EXECUTABLE_SRCS 
	CliRunnable.h
	DynamicModules.h
	Master.h
	WorldRunnable.h
	CliRunnable.cpp
	DynamicModules.cpp
	Main.cpp
	Master.cpp
	WorldRunnable.cpp

)


if(ENABLE_SOAP)
  list(APPEND EXECUTABLE_SRCS
    MaNGOSsoap.h
    MaNGOSsoap.cpp
    soap/soapStub.h
    soap/soapH.h
    soap/soapC.cpp
    soap/soapServer.cpp
  )
endif()

if(WIN32)
  list(APPEND EXECUTABLE_SRCS
    mangosd.rc
  )
  
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D__ACE_INLINE__")
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
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
  ${CMAKE_SOURCE_DIR}/src/game/MapNodes
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
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/soap
  ${CMAKE_SOURCE_DIR}/dep/include/gsoap
  ${CMAKE_BINARY_DIR}/src/shared
  ${CMAKE_BINARY_DIR}
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

if(WIN32)
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/include-windows
  )
endif()

if (ENABLE_PROFILING)
    include_directories(
        ${OPTICK_INCLUDE_DIR}
    )
endif()

# Display some debug informations at server startup
set(DEFINITIONS
  ${DEFINITIONS}
  MANGOS_ALLOC_LIB="${ALLOC_LIB_INFO_STRING}"
)
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "${DEFINITIONS}")

add_executable(${EXECUTABLE_NAME}
  ${EXECUTABLE_SRCS}
)

if(USE_SCRIPTS)
  target_link_libraries(${EXECUTABLE_NAME}
    game
    scripts
    shared
    framework
    g3dlite
    ${ACE_LIBRARIES}
  )
else()
  target_link_libraries(${EXECUTABLE_NAME}
    game
    #scripts
    shared
    framework
    g3dlite 
    ${ACE_LIBRARIES}
  )
endif()

if(USE_DISCORD_BOT)
    target_link_libraries(${EXECUTABLE_NAME}
	dpp
  )
endif()

target_link_libraries(${EXECUTABLE_NAME}
    libdeflate
)

# SOAP remote-command interface: vendored gsoap C++ runtime (2.8.135).
# Linux-only (vendored static lib); guard behind ENABLE_SOAP so Windows can build.
if(ENABLE_SOAP)
  target_compile_definitions(${EXECUTABLE_NAME} PRIVATE ENABLE_SOAP)
  target_link_libraries(${EXECUTABLE_NAME}
      ${CMAKE_SOURCE_DIR}/dep/lib/gsoap/libgsoap++.a
  )
endif()


if(WIN32)
  target_link_libraries(${EXECUTABLE_NAME}
    zlib
    optimized ${MYSQL_LIBRARY}
    optimized ${OPENSSL_LIBRARIES}
    debug ${MYSQL_DEBUG_LIBRARY}
    debug ${OPENSSL_DEBUG_LIBRARIES}
  )
  if(MINGW)
    target_link_libraries(${EXECUTABLE_NAME}
      -lws2_32
    )
  endif()

  if (ENABLE_PROFILING)
      target_link_libraries(${EXECUTABLE_NAME}
        ${OPTICK_LIBRARY})
  endif()

  if(PLATFORM MATCHES X86)
    target_link_libraries(${EXECUTABLE_NAME})
  endif()
endif()

if(UNIX)
  target_link_libraries(${EXECUTABLE_NAME}
    ${MYSQL_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${OPENSSL_EXTRA_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${CMAKE_DL_LIBS}
  )
endif()

set(EXECUTABLE_LINK_FLAGS "")

if(UNIX)
  set(EXECUTABLE_LINK_FLAGS "-pthread ${EXECUTABLE_LINK_FLAGS} -rdynamic")
endif()

if(APPLE)
  set(EXECUTABLE_LINK_FLAGS "-framework Carbon ${EXECUTABLE_LINK_FLAGS}")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS
  "${EXECUTABLE_LINK_FLAGS}"
)

install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${BIN_DIR})
install(FILES run-mangosd DESTINATION ${BIN_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mangosd.conf.dist.in ${CMAKE_CURRENT_BINARY_DIR}/mangosd.conf.dist)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mangosd.conf.dist DESTINATION ${CONF_DIR})

if(WIN32 AND MSVC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/\${BUILD_TYPE}/${EXECUTABLE_NAME}.pdb DESTINATION ${BIN_DIR} CONFIGURATIONS Debug)
endif()
