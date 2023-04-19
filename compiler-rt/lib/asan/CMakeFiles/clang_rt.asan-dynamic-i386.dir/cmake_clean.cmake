file(REMOVE_RECURSE
  "../linux/libclang_rt.asan-i386.pdb"
  "../linux/libclang_rt.asan-i386.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang ASM CXX)
  include(CMakeFiles/clang_rt.asan-dynamic-i386.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
