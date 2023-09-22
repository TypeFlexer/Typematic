void foo(void) {
  char *s = "hello \"as if\" he said";
  _Nt_array_ptr<char> s2 = "hello \"as \
    if\" he said";
  char *p = "_Ptr<int> is in a string---should stay!";
  char x = '\'';
  char y = '\"';
  if (y == '\\' || 0) return;
   // TODO: compiler isn't constant folding when checking bounds, so we need the spurious (size_t) 1 here.
  _Ptr<int> q = 0;
        // TODO: Can't prove this
   _Unchecked { int x = 0; }

}
