file(REMOVE_RECURSE
  "../linux/libclang_rt.fuzzer-i386.a"
  "../linux/libclang_rt.fuzzer-i386.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/clang_rt.fuzzer-i386.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
