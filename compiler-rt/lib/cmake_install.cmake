# Install script for directory: /home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib

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
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/sanitizer_common/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/builtins/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/crt/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/interception/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/stats/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/lsan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/ubsan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/asan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/dfsan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/msan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/hwasan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/tsan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/tsan/dd/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/safestack/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/cfi/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/scudo/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/scudo/standalone/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/ubsan_minimal/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/gwp_asan/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/profile/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/xray/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/fuzzer/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/memprof/cmake_install.cmake")
  include("/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/scudo/standalone/fuzz/cmake_install.cmake")

endif()

