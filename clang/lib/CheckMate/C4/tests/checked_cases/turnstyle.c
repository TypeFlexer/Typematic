int *p : itype
  (_Array_ptr<int>
  ) count(127) = 0;

int foo(void) { 
  int e = p? 1: 23;
  return e;
}
