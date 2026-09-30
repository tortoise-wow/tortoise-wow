/*
 * Forced into every Windows dynamic module's compile (/FI, modules/CMakeLists.txt).
 *
 * A module on Windows is a DLL that imports the core from mangosd.exe (ModuleExports.cmake). The
 * core's singletons must be the core's: a module that instantiated one of them itself would hold a
 * private copy -- its sLog writing nowhere, its script registrations filling a registry the server
 * never reads. A singleton's Instance() is defined in a header (SingletonImp.h, pulled in by
 * FactoryHolder.h), so a module compiles its own unless told the core already has it. Each line
 * below tells it: an explicit instantiation declaration, answered by the core's INSTANTIATE_SINGLETON.
 *
 * Only the singletons a module's code instantiates need a line -- the rest it reaches through
 * functions the core exports already. A module that needs another adds it here.
 */

#ifndef TORTOISE_MODULE_IMPORTS_H
#define TORTOISE_MODULE_IMPORTS_H

#include "Log.h"

extern template class MaNGOS::Singleton<Log, MaNGOS::ClassLevelLockable<Log, std::mutex> >;

#endif
