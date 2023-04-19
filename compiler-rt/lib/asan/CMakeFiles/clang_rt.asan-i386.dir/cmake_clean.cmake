file(REMOVE_RECURSE
  "../linux/libclang_rt.asan-i386.a"
  "../linux/libclang_rt.asan-i386.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang ASM CXX)
  include(CMakeFiles/clang_rt.asan-i386.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
