//#include <stdio.h>
#include <stdlib.h>

////#include <stdnoreturn.h>
#include <_builtin_stdio_tainted.h>
#include <stdio_tainted.h>
#include <stdlib_tainted.h>
#include <stdtainted.h>
//#pragma CHECKED ON
//// Function to sort the numbers using poin
////_Tainted _TPtr<int> tainted_sort (int n, _TArray_ptr<int> ptr);

Tstruct twin_turbo {
  int turbo_1;
  int turbo_2;
  _TArray_ptr<char> engine : count(102);
  struct twin_turbo another;
};

struct Foo _For_any(T) {
  T *x;
};

Tstruct tainted_twin_turbo {
  int turbo_1;
  int turbo_2;
  int a;
  int b;
  int c;
  int d;
  _TPtr<int> checked_pointer;
};

_TArray_ptr<int> t48 _Checked[3][2] = {0};
_TPtr<int> *t47 _Checked[10][15];

void checked_local_types(void) _Checked { _TPtr<int> t11 = 0; }

int sample(int a, ...);
_Tainted int g_tainted_sort(int n, _TArray_ptr<int> ptr, Tstruct twin_turbo tb);
_Tainted _TPtr<int> tainted_sort(int n, _TArray_ptr<int> ptr,
                                 _TPtr<Tstruct twin_turbo> tb) {

  print("hello Forbidden Fruit\n");
  return NULL;
}
int a = 10;
int b = 10;

void client_register() {
  b = a + 201;
  return;
}

// Driver code
int main(void) {
  int n = 6;
  _TArray_ptr<int> temp : count(10) =
                              (_TArray_ptr<int>)t_malloc(10 * sizeof(int));
  // Fill whole array with 0.
  _Checked {
    //t_memset <int>(temp, 2, 10 * sizeof(int));
  }
  char buffer[50];
  _TArray_ptr<char> tainted_buffer =
      (_TArray_ptr<char>)t_malloc(50 * sizeof(char));
  int a = 10, b = 20, c;
  c = a + b;
  _TArray_ptr<char> tainted_format =
      (_TArray_ptr<char>)t_malloc(50 * sizeof(char));
  t_strcpy(tainted_format, "Sum of %d and %d is %d");
  t_printf("hey yall %s", tainted_format);
  __t_sprintf_chkcbx(tainted_buffer, 10, 10, tainted_format, a, b, c);
  client_register();
  temp[0] = 0;
  temp[1] = 23;
  temp[2] = 14;
  temp[3] = 39;
  temp[4] = 12;
  temp[5] = 9;

  int *host_pointer = (int *)malloc(10 * sizeof(int));
  host_pointer[0] = 0;
  if (!c_isPointerToTaintedMem((void *)host_pointer)) {
    printf("\nPointer belongs to Host memory and has data %d", host_pointer[0]);
  }

  Tstruct twin_turbo tb2;
  tb2.turbo_1 = 10;
  tb2.turbo_2 = 20;
  // Yeah, this sort of works -->
  _TPtr<int> r34 = NULL;
  int *leaked_pointer = (int *)malloc(10 * sizeof(int));
  _TPtr<Tstruct twin_turbo> tb =
      (_TPtr<Tstruct twin_turbo>)t_malloc(sizeof(Tstruct twin_turbo));
  *tb = tb2;
  tb->engine = (_TArray_ptr<char>)t_malloc(102 * sizeof(char));
  _Array_ptr<char> engine_check
      : count(102) = (_Array_ptr<char>)malloc(102 * sizeof(char));
  char *very = (char *)malloc(20 * sizeof(char));
  strcpy(very, "Guten Tag!, Wie bist du?");
  _Unchecked { t_strcpy(tb->engine, very); }
  _TPtr<int> r34 = tainted_sort(n, temp, tb);
  t_printf("value is %d ", *r34);
  /*printf(" %d", r34[0]);
    printf(" %d", r34[1]);
    printf(" %d", r34[2]);
    printf(" %d", r34[3]);
    printf(" %d", r34[4]);
    printf(" %d", r34[5]);
*/
  return 0;
}
