_Itype_for_any(U) void test(void *p : itype(_Ptr<U>));
_Itype_for_any(T) void test(void *p : itype(_Ptr<T>)) {
  int U = 5;
  _Ptr<T> tmp = p;
}
void foo(_Ptr<int> p) { _Ptr<int> T = p; }
