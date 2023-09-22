struct foo {
  _Array_ptr<_Ptr<int>> items : count(cnt);
  int cnt;
};
int *bar(void);
int *foo(int x, int y);
void f(int *n, struct foo *p) {
  int *x = _Assume_bounds_cast<_Array_ptr<int>>(n, bounds(n, n + 5));
  int *x1 = _Assume_bounds_cast<_Array_ptr<int>>(bar());
  int *x2 = _Assume_bounds_cast<_Array_ptr<int>>(foo(1, 2));
  int *y = _Assume_bounds_cast<_Ptr<int>>(n);
  void *z = _Dynamic_bounds_cast<_Array_ptr<_Ptr<int>>>(
      p->items, byte_count(p->cnt * sizeof(_Ptr<int>)));
}
