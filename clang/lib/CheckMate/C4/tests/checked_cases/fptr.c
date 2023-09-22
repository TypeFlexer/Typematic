void foo(void) {
  _Ptr<int(unsigned int)> mapfunc = 0;
  _Ptr<int(int)> mapfunc2 = 0;
  _Ptr<int(volatile unsigned int)> mapfunc3 = 0;
  _Ptr<int(volatile unsigned int)> mapfunc4 = 0;
  _Ptr<const int(volatile unsigned int)> mapfunc5 = 0;
  void fn(int((*f)(void)) : itype(_Ptr<int(void)>));
  // should translate to
  // int (*mapfunc)(unsigned int) = 0;
}

_Itype_for_any(T) void json_set_allocation_functions(_Ptr<void* (int s) : itype(_Array_ptr<T>) byte_count(s)> malloc,
    _Ptr<void (void* : itype(_Array_ptr<T>) byte_count(0))> free);
