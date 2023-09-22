#include <stdchecked.h>

ptr<int> foo(ptr<int> p) {
  *p = 1;
  return (_Ptr<int>)0;
}
