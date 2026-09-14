---
layout: default
title: Building This Project
---

# Building This Project

This page explains how to compile the Tortoise WoW server from source.

The project uses CMake and must be built outside the source directory. The
examples below create a separate `build` directory and install the compiled
server into a separate `server` directory.

## Common Requirements

You need:

- Git
- CMake
- A C++17 compiler
- MySQL or MariaDB client development files
- OpenSSL development files
- zlib development files
- bzip2 development files
- ACE development files

TBB is optional by default. The project uses standard malloc unless you configure
with `-DUSE_STD_MALLOC=OFF`.

Useful CMake options:

| Option | Default | Description |
| --- | --- | --- |
| `-DPREFIX=../server` | `../server` | Install directory for binaries and config templates. |
| `-DUSE_EXTRACTORS=ON` | `OFF` | Build map and DBC extraction tools. |
| `-DMODULES=disabled` | `disabled` | Module build mode. Use `static` or `dynamic` to enable modules. |
| `-DUSE_STD_MALLOC=ON` | `ON` | Use standard malloc instead of TBB malloc. |

## Windows

Recommended tools:

- Visual Studio 2022 with the **Desktop development with C++** workload
- Git for Windows
- CMake
- ACE built for the same architecture as your server build

The repository includes Windows copies of several libraries under `dep/windows`,
but ACE is still required separately. If you don't already have a working ACE
build, [vcpkg](https://vcpkg.io/) is one convenient way to get one — see the
optional section below — but any ACE build for your architecture works.

1. Open **Developer PowerShell for VS 2022**.
2. Clone the repository:

   ```powershell
   git clone https://github.com/tortoise-wow/tortoise-wow.git tortoise-wow
   cd tortoise-wow
   ```

3. Configure the build. Replace `C:\deps\ACE` with your ACE install path:

   ```powershell
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=server -DACE_ROOT=C:\deps\ACE
   ```

4. Build the server:

   ```powershell
   cmake --build build --config Release
   ```

5. Install the compiled files and config templates:

   ```powershell
   cmake --install build --config Release
   ```

The installed files will be placed in the `server` directory unless you used a
different `PREFIX`.

### Optional: getting ACE via vcpkg

Building ACE for Windows by hand is not entirely trivial. [vcpkg](https://vcpkg.io/)
is one convenient, optional way to obtain a working build instead:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\deps\vcpkg
C:\deps\vcpkg\bootstrap-vcpkg.bat
C:\deps\vcpkg\vcpkg.exe install ace:x64-windows
```

This installs ACE under `C:\deps\vcpkg\installed\x64-windows` — use that as
`-DACE_ROOT` in step 3 above instead of a hand-built ACE.

## Ubuntu

Install dependencies:

```sh
sudo apt update
sudo apt install git build-essential cmake libace-dev default-libmysqlclient-dev libssl-dev zlib1g-dev libbz2-dev
```

Clone and build:

```sh
git clone https://github.com/tortoise-wow/tortoise-wow.git tortoise-wow
cd tortoise-wow
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server
cmake --build build --parallel
cmake --install build
```

If you enable TBB malloc with `-DUSE_STD_MALLOC=OFF`, also install:

```sh
sudo apt install libtbb-dev
```

## Debian

Install dependencies:

```sh
sudo apt update
sudo apt install git build-essential cmake libace-dev default-libmysqlclient-dev libssl-dev zlib1g-dev libbz2-dev
```

Clone and build:

```sh
git clone https://github.com/tortoise-wow/tortoise-wow.git tortoise-wow
cd tortoise-wow
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server
cmake --build build --parallel
cmake --install build
```

If you enable TBB malloc with `-DUSE_STD_MALLOC=OFF`, also install:

```sh
sudo apt install libtbb-dev
```

## Fedora

Install dependencies:

```sh
sudo dnf install git gcc-c++ make cmake ACE-devel mariadb-connector-c-devel openssl-devel zlib-devel bzip2-devel
```

Clone and build:

```sh
git clone https://github.com/tortoise-wow/tortoise-wow.git tortoise-wow
cd tortoise-wow
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server
cmake --build build --parallel
cmake --install build
```

If you enable TBB malloc with `-DUSE_STD_MALLOC=OFF`, also install:

```sh
sudo dnf install tbb-devel
```

## Arch Linux

Install dependencies:

```sh
sudo pacman -Syu --needed git base-devel cmake ace mariadb-libs openssl zlib bzip2
```

Clone and build:

```sh
git clone https://github.com/tortoise-wow/tortoise-wow.git tortoise-wow
cd tortoise-wow
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server
cmake --build build --parallel
cmake --install build
```

If you enable TBB malloc with `-DUSE_STD_MALLOC=OFF`, also install:

```sh
sudo pacman -S --needed onetbb
```

## Building Extractor Tools

Extractor tools are disabled by default. Enable them when configuring CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server -DUSE_EXTRACTORS=ON
cmake --build build --parallel
cmake --install build
```

## Building Modules

Modules are disabled by default. To build discovered modules statically:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server -DMODULES=static
cmake --build build --parallel
cmake --install build
```

To build modules dynamically, use:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPREFIX=../server -DMODULES=dynamic
```

Individual modules can be controlled with generated options such as
`-DMODULE_MOD_EXAMPLE=static`.

## After Building

Before starting the server, review the installed `.conf.dist` files, copy the
ones you need to matching `.conf` filenames, and update database connection
settings.

Database setup is separate from compilation. Import `sql/create_databases.sql`,
then import the SQL files in `sql/base`, then let `mangosd` apply tracked
updates on startup.

## Troubleshooting

If CMake cannot find ACE, pass the ACE install path explicitly:

```sh
cmake -S . -B build -DACE_ROOT=/path/to/ace
```

If CMake cannot find MySQL or MariaDB on Linux, confirm that the development
package is installed and that `mysql_config` is available in your `PATH`.

If you accidentally configure in the source directory, delete the generated
`CMakeCache.txt` and `CMakeFiles` from the source directory, then configure
again with `cmake -S . -B build`.

## Next Step

Continue to [Database Setup](database-setup.md).
