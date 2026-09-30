# What Windows (MSVC) dynamic modules need from mangosd.exe, written as the .def it links with.
#
#   cmake -DDUMPBIN=<dumpbin.exe> -DOBJECTS=<obj|obj> -DLIBS=<lib|lib> -DOUT=<file.def>
#         -P ModuleExports.cmake
#
# (The lists are "|"-separated: a custom command hands them over as one argument each.)
#
# Linux gets this for nothing: mangosd links with -rdynamic and a module resolves against the
# running core. On Windows an executable exports nothing unless told, and it cannot be told
# "everything" -- game alone defines ~88 000 symbols, and an export table holds 65 535. So it
# exports exactly what the modules ask for: every symbol their objects leave undefined that the
# core's libraries define. A module that asks for something new relinks mangosd with it.
#
# How a symbol goes out is read from the module's own object. A function the module calls
# (dumpbin: "notype ()") goes out plain: the import library gives it a call thunk. Anything else
# goes out DATA -- the import library gives it only its __imp_ address. A module reaches the
# core's data through __declspec(dllimport) (TW_CORE_DATA, ScriptMgr.h), which asks for exactly
# that address (the name "__imp_<name>"); a module that reads a core global without it fails to
# link, loudly, instead of reading a thunk's bytes as the variable.

foreach(var DUMPBIN OBJECTS LIBS OUT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "ModuleExports.cmake: ${var} not given")
  endif()
endforeach()
string(REPLACE "|" ";" OBJECTS "${OBJECTS}")
string(REPLACE "|" ";" LIBS "${LIBS}")

set(undefined)
set(called)
set(defined_here)
foreach(obj ${OBJECTS})
  execute_process(COMMAND "${DUMPBIN}" /nologo /symbols "${obj}" OUTPUT_VARIABLE dump)
  # dumpbin ends its lines "\r\n": a name stops at either.
  string(REGEX MATCHALL "UNDEF[^\n|]*External[ ]+\\| [^ \r\n]+" hits "${dump}")
  foreach(hit ${hits})
    string(REGEX REPLACE ".*\\| " "" name "${hit}")
    if(name MATCHES "^__imp_")
      string(SUBSTRING "${name}" 6 -1 name)
    elseif(hit MATCHES "\\(\\)")
      list(APPEND called "${name}")
    endif()
    list(APPEND undefined "${name}")
  endforeach()
  string(REGEX MATCHALL "SECT[0-9A-F]+[^\n|]*External[ ]+\\| [^ \r\n]+" hits "${dump}")
  foreach(hit ${hits})
    string(REGEX REPLACE ".*\\| " "" name "${hit}")
    list(APPEND defined_here "${name}")
  endforeach()
endforeach()
list(REMOVE_DUPLICATES undefined)
if(defined_here)
  list(REMOVE_ITEM undefined ${defined_here})
endif()

# The core's public symbols: each library's first linker member -- one "<offset> <name>" line
# per symbol -- kept as text and searched for each name the modules ask about. (A list of the
# ~100 000 names, searched with list(FIND), took six minutes; a literal find takes seconds.)
set(core "")
foreach(lib ${LIBS})
  execute_process(COMMAND "${DUMPBIN}" /nologo /linkermember:1 "${lib}" OUTPUT_VARIABLE dump)
  string(REPLACE "\r" "" dump "${dump}")
  string(APPEND core "${dump}")
endforeach()

set(functions 0)
set(data 0)
set(def "EXPORTS\n")
foreach(name ${undefined})
  string(FIND "${core}" " ${name}\n" at)
  if(at EQUAL -1)
    continue()
  endif()
  list(FIND called "${name}" is_called)
  if(NOT is_called EQUAL -1)
    string(APPEND def "    ${name}\n")
    math(EXPR functions "${functions} + 1")
  else()
    string(APPEND def "    ${name} DATA\n")
    math(EXPR data "${data} + 1")
  endif()
endforeach()

file(WRITE "${OUT}.new" "${def}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${OUT}.new" "${OUT}")
message(STATUS "mangosd exports for the dynamic modules: ${functions} functions, ${data} data")
