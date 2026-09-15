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

# Glob only and not recurse, there are other libs for that
set (shared_SRCS 
    ByteBuffer.h
    Common.h
    DelayExecutor.h
    Errors.h
    LockedQueue.h
    Log.h
    httplib.h
    PerfStats.h
    PosixDaemon.h
    revision.h
    SystemConfig.h
    ThreadPool.h
    Timer.h
    Util.h
    AllocatorWithCategory.h
    OptickIntegration.h
    WorldPacket.h
    Auth/ARC4.h
    Auth/AuthCrypt.h
    Auth/base32.h
    Auth/BigNumber.h
    Auth/Hmac.h
    Auth/HMACSHA1.h
    Auth/md5.h
    Auth/Sha1.h
    Config/Config.h
    Config/ConfigEnv.h
    Database/AutoUpdater.hpp
    Database/Database.h
    Database/DatabaseEnv.h
    Database/DatabaseImpl.h
    Database/DatabaseMysql.h
    Database/DatabasePostgre.h
    Database/DBCFileLoader.h
    Database/DBCStore.h
    Database/Field.h
    Database/MySQLDelayThread.h
    Database/PGSQLDelayThread.h
    Database/QueryResult.h
    Database/QueryResultMysql.h
    Database/QueryResultPostgre.h
    Database/SqlDelayThread.h
    Database/SqlOperations.h
    Database/SqlPreparedStatement.h
    Database/SQLStorage.h
    Database/SQLStorageImpl.h
    Common.cpp
    DelayExecutor.cpp
    Log.cpp
    PerfStats.cpp
    PosixDaemon.cpp
    ThreadPool.cpp
    BS_thread_pool.hpp
    Util.cpp
    Timer.cpp
    Duration.h
    Auth/ARC4.cpp
    Auth/AuthCrypt.cpp
    Auth/base32.cpp
    Auth/BigNumber.cpp
    Auth/Hmac.cpp
    Auth/HMACSHA1.cpp
    Auth/Sha1.cpp
    Config/Config.cpp
    Database/AutoUpdater.cpp
    Database/Database.cpp
    Database/DatabaseMysql.cpp
    Database/DatabasePostgre.cpp
    Database/DBCFileLoader.cpp
    Database/Field.cpp
    Database/QueryResultMysql.cpp
    Database/QueryResultPostgre.cpp
    Database/SqlDelayThread.cpp
    Database/SqlOperations.cpp
    Database/SqlPreparedStatement.cpp
    Database/SQLStorage.cpp
    HttpApi/ApiServer.hpp
    HttpApi/ApiServer.cpp
    HttpApi/BaseController.cpp
    HttpApi/BaseController.hpp
    HttpApi/Authorizers/ApiKeyAuthorizer.hpp
    HttpApi/Authorizers/BaseAuthorizer.hpp
)


if(USE_DISCORD_BOT)
    list(APPEND shared_SRCS
      DiscordBot/BaseCommandHandler.hpp
      DiscordBot/BaseCommandHandler.cpp
      DiscordBot/AuthManager.cpp
      DiscordBot/AuthManager.hpp
      DiscordBot/Bot.cpp
      DiscordBot/Bot.hpp
      DiscordBot/MiscUtil.hpp
      DiscordBot/toojpeg.hpp
      DiscordBot/toojpeg.cpp
      DiscordBot/QrCodeGen.hpp
      DiscordBot/QrCodeGen.cpp	 
    )
endif()

if(USE_LIBCURL)
  set (shared_SRCS 
    ${shared_SRCS}
    SendgridMail.h
    MailerService.h
    SendgridMail.cpp
    MailerService.cpp
  )
endif()

# Exclude Win32 files
if(WIN32)
  list(REMOVE_ITEM shared_SRCS
    PosixDaemon.h
    PosixDaemon.cpp
    revision.h
  )
  
	if (ENABLE_PROFILING)
		include_directories(
			${OPTICK_INCLUDE_DIR}
    )
	endif()

    if (NOT MSVC)
      list(REMOVE_ITEM shared_SRCS
        WheatyExceptionReport.cpp
        WheatyExceptionReport.h
      )
    endif()

    set (shared_SRCS 
    ${shared_SRCS}
    Turtle.natvis
    )

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D__ACE_INLINE__")
else()
  list(REMOVE_ITEM shared_SRCS
    WheatyExceptionReport.cpp
    WheatyExceptionReport.h
    ServiceWin32.cpp
    ServiceWin32.h
    revision.h
  )
endif()

source_group("Util"
  REGULAR_EXPRESSION .*
)

foreach(SRC ${shared_SRCS})
  get_filename_component(PTH ${SRC} PATH)
  if(PTH)
    if(NOT XCODE) # FIXME: Xcode Generator has bug with nested dirs
      string(REPLACE "/" "\\\\" PTH ${PTH})
    endif()
    source_group(${PTH} FILES ${SRC})
  endif()
endforeach(SRC)

source_group("DataStores"
  REGULAR_EXPRESSION DBC
)

source_group("Log"
  REGULAR_EXPRESSION Log
)

source_group("DiscordBot"
  REGULAR_EXPRESSION DiscordBot
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/libdeflate
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_BINARY_DIR}
  # OPENSSL_INCLUDE_DIR before ACE_INCLUDE_DIR, not after: on a Windows build with -DACE_ROOT
  # pointed at a vcpkg installed tree, ACE_INCLUDE_DIR IS that tree's shared include/ folder -
  # vcpkg lays every package for a triplet under one include/ directory, so it also contains
  # vcpkg's own OpenSSL 3.x headers, not just ACE's. The compiler searches -I paths in the
  # order given, so with ACE_INCLUDE_DIR first, "#include <openssl/x.h>" always resolved
  # through vcpkg's 3.x headers there, no matter what OPENSSL_INCLUDE_DIR itself pointed at.
  ${OPENSSL_INCLUDE_DIR}
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
)

if(WIN32)
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/include-windows
  )
endif()

add_library(shared STATIC
  ${shared_SRCS}
)

if(USE_DISCORD_BOT)
    target_link_libraries(shared PUBLIC
	dpp
  )
  target_compile_definitions(shared PUBLIC USING_DISCORD_BOT)
endif()


if(UNIX)
  find_package(Threads)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
      message(WARNING "Core requires GCC of 8 or higher.")
    elseif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1)
      target_link_libraries(shared PUBLIC stdc++fs)
    endif()
  endif()

  target_link_libraries(shared PUBLIC ${ACE_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
endif(UNIX)

if(MINGW)
  target_link_libraries(shared PUBLIC ${ACE_LIBRARIES} -lws2_32)
endif()

target_link_libraries(shared PUBLIC re2::re2)
target_link_libraries(shared PUBLIC fmt)


SET_TARGET_PROPERTIES (shared PROPERTIES FOLDER "Game Libs")
