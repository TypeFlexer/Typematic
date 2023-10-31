# Install script for directory: /home/arun/Desktop/checkedc/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xcheckedc-headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/include" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/home/arun/Desktop/checkedc/include/assert_checked.h"
    "/home/arun/Desktop/checkedc/include/assert.h"
    "/home/arun/Desktop/checkedc/include/errno_checked.h"
    "/home/arun/Desktop/checkedc/include/errno.h"
    "/home/arun/Desktop/checkedc/include/fenv_checked.h"
    "/home/arun/Desktop/checkedc/include/fenv.h"
    "/home/arun/Desktop/checkedc/include/inttypes_checked.h"
    "/home/arun/Desktop/checkedc/include/inttypes_checked_internal.h"
    "/home/arun/Desktop/checkedc/include/math_checked.h"
    "/home/arun/Desktop/checkedc/include/math.h"
    "/home/arun/Desktop/checkedc/include/signal_checked.h"
    "/home/arun/Desktop/checkedc/include/signal.h"
    "/home/arun/Desktop/checkedc/include/stdchecked.h"
    "/home/arun/Desktop/checkedc/include/stdio_checked.h"
    "/home/arun/Desktop/checkedc/include/stdio.h"
    "/home/arun/Desktop/checkedc/include/stdlib_checked.h"
    "/home/arun/Desktop/checkedc/include/stdlib_tainted.h"
    "/home/arun/Desktop/checkedc/include/stdlib.h"
    "/home/arun/Desktop/checkedc/include/string_checked.h"
    "/home/arun/Desktop/checkedc/include/string_tainted.h"
    "/home/arun/Desktop/checkedc/include/string.h"
    "/home/arun/Desktop/checkedc/include/threads_checked.h"
    "/home/arun/Desktop/checkedc/include/threads.h"
    "/home/arun/Desktop/checkedc/include/time_checked.h"
    "/home/arun/Desktop/checkedc/include/time.h"
    "/home/arun/Desktop/checkedc/include/unistd_checked.h"
    "/home/arun/Desktop/checkedc/include/unistd.h"
    "/home/arun/Desktop/checkedc/include/_builtin_stdio_checked.h"
    "/home/arun/Desktop/checkedc/include/_builtin_string_checked.h"
    "/home/arun/Desktop/checkedc/include/_builtin_common.h"
    "/home/arun/Desktop/checkedc/include/checkedc_extensions.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xclang-headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/include/arpa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/home/arun/Desktop/checkedc/include/arpa/inet_checked.h"
    "/home/arun/Desktop/checkedc/include/arpa/inet.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xclang-headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/include/sys" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/home/arun/Desktop/checkedc/include/sys/socket_checked.h"
    "/home/arun/Desktop/checkedc/include/sys/socket.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/arun/Desktop/checkedc/include/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
