struct foo {
  _Ptr<int> p;
};

#define foo(t,p) _Ptr<t> p

_Array_ptr<int> p : count(4);

int main(void) {
  int *x = 0;
  _Ptr<int> p = (_Ptr<int>)0;
}
