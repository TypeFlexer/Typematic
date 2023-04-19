file(REMOVE_RECURSE
  "../linux/libclang_rt.ubsan_standalone-i386.pdb"
  "../linux/libclang_rt.ubsan_standalone-i386.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/clang_rt.ubsan_standalone-dynamic-i386.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
