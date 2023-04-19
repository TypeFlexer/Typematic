
set(CMAKE_C_COMPILER "/usr/bin/cc" CACHE STRING "Initial cache" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/c++" CACHE STRING "Initial cache" FORCE)
set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Initial cache" FORCE)
set(CMAKE_INSTALL_PREFIX "/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/fuzzer/libcxx_fuzzer_i386-bins/cxx/benchmarks/benchmark-libcxx" CACHE PATH "Initial cache" FORCE)
set(CMAKE_CXX_FLAGS "-Wno-unused-command-line-argument -nostdinc++ -isystem /home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/libcxx/include -L/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/fuzzer/libcxx_fuzzer_i386/lib -Wl,-rpath,/home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/fuzzer/libcxx_fuzzer_i386/lib -L -Wl,-rpath, -include /home/twinturbo/Desktop/ExtraSpace/CheckCBox_Compiler/compiler-rt/lib/fuzzer/libcxx_fuzzer_i386-bins/cxx/__config_site" CACHE STRING "Initial cache" FORCE)
set(BENCHMARK_USE_LIBCXX "ON" CACHE BOOL "Initial cache" FORCE)
set(BENCHMARK_ENABLE_TESTING "OFF" CACHE BOOL "Initial cache" FORCE)