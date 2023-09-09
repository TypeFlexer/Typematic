#include <stdio.h>
#include <stdchecked.h>

void foo(ptr<int> p) {
  *p = 1;
  array_ptr<char> q : bounds(5) = 0;
}
