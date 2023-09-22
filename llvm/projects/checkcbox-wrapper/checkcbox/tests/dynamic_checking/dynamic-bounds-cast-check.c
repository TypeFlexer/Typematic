// The following lines are for the clang automated test suite
//
// RUN: %clang_cc1 -verify -verify-ignore-unexpected=warning -verify-ignore-unexpected=note %s

#include <stdchecked.h>

void passing_test_1(void);
void passing_test_2(void);
void passing_test_3(void);

void failing_test_1(void);
void failing_test_2(void);
void failing_test_3(void);
void failing_test_4(int k);
void failing_test_5(_TArray_ptr<char> pc : count(len), unsigned len);
void failing_test_6(_TArray_ptr<char> pc : count(len), unsigned len);
void failing_test_7(_TArray_ptr<char> pc : count(len), unsigned len);
void failing_test_8(unsigned len);


// This signature for main is exactly what we want here,
// it also means any uses of argv[i] are checked too!
int main(int argc, _Array_ptr<char*> argv : count(argc)) {

  int a _Checked[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  if (strcmp(argv[1], "pass1") == 0) {
    // CHECK-PASS-1: Printable0
    // CHECK-PASS-1: Printable1
    // CHECK-PASS-1: Printable2
    // CHECK-PASS-1: Printable3
    // CHECK-PASS-1: Printable4
    // CHECK-PASS-1: Expected Success
    passing_test_1();
  }
  else if (strcmp(argv[1], "pass2") == 0) {
    // CHECK-PASS-2: Printable0
    // CHECK-PASS-2: Printable1
    // CHECK-PASS-2: Printable2
    // CHECK-PASS-2: Printable3
    // CHECK-PASS-2: Expected Success
    passing_test_2();
  }
  else if (strcmp(argv[1], "pass3") == 0) {
    // CHECK-PASS-3: Passed p1
    // CHECK-PASS-3: Passed p2
    // CHECK-PASS-3: Passed p3
    // CHECK-PASS-3: Passed p4
    // CHECK-PASS-3: Expected Success
    passing_test_3();
  } else if (strcmp(argv[1], "fail1") == 0) {
    // CHECK-FAIL-1-NOT: Unprintable
    // CHECK-FAIL-1-NOT: Unexpected Success
    failing_test_1();
  }
  else if (strcmp(argv[1], "fail2") == 0) {
    // CHECK-FAIL-2-NOT: Unprintable
    // CHECK-FAIL-2-NOT: Unexpected Success
    failing_test_2();
  }
  else if (strcmp(argv[1], "fail3") == 0) {
    // CHECK-FAIL-3 : Printable0
    // CHECK-FAIL-3 : Printable1
    // CHECK-FAIL-3 : Printable2
    // CHECK-FAIL-3-NOT: Unprintable
    // CHECK-FAIL-3-NOT: Unexpected Success
    failing_test_3();
  } else if (strcmp(argv[1], "fail4") == 0) {
    // CHECK-FAIL-4 : Printable1
    // CHECK-FAIL-4-NOT: Unprintable2
    // CHECK-FAIL-4-NOT: Unexpected Success
    failing_test_4(5);
  } else if (strcmp(argv[1], "fail5") == 0) {
    _TArray_ptr<char> p : count(12) = "\0\0\0\0\0\0\0\0abcd"; //expected-error {{initializing '_TArray_ptr<char>' with an expression of incompatible type 'char [13]'}}
    // CHECK-FAIL-5: Successful pointer conversion
    // CHECK-FAIL-5-NOT: Unexpected Success
    failing_test_5(p, sizeof(int) + 1);
    failing_test_5(p, sizeof(int) - 1);
  } else if (strcmp(argv[1], "fail6") == 0) {
    _TArray_ptr<char> p : count(4) = "abcd"; //expected-error {{initializing '_TArray_ptr<char>' with an expression of incompatible type 'char [5]'}}
    // CHECK-FAIL-6: Successful conversion to_TPtr<void>
    // CHECK-FAIL-6-NOT: Unexpected Success
    failing_test_6(p, 1);
    failing_test_6(p, 0);
  } else if (strcmp(argv[1], "fail7") == 0) {
    _TArray_ptr<char> p : count(4) = "abcd"; //expected-error {{initializing '_TArray_ptr<char>' with an expression of incompatible type 'char [5]'}}
    // CHECK-FAIL-7: Successful conversion to void *
    // CHECK-FAIL-7-NOT: Unexpected Success
    failing_test_7(p, 1);
    failing_test_7(p, 0);
  } else if (strcmp(argv[1], "fail8") == 0) {
    // CHECK-FAIL-8: Successful conversion to _TNt__TArray_ptr<const char>
    // CHECK-FAIL-8-NOT: Unexpected Success
    failing_test_8(5);
    failing_test_8(7);
  } else {
    // CHECK-NOT: Unexpected Test Name
    printf("Unexpected Test Name");
  }

  // CHECK-PASS: All Dynamic Checks Passed
  // CHECK-FAIL-NOT: All Dynamic Checks Passed
  printf("All Dynamic Checks Passed");
  return 0;
}

// dynamic_check(r != NULL) && dynamic_check(r <= r && r+1 <= r+10) - > OK
// dynamic_check(r != NULL) && dynamic_check(r <= r && r+3 <= r+10) - > OK
// dynamic_check((r+3) != NULL) && dynamic_check(r <= r+3 && r+6 <= r+10) - > OK
// dynamic_check((r+3) != NULL) && dynamic_check(r <= r && r+15 <= r+10) - > OK
// dynamic_check("abcdef", count(2)) -> OK.
void passing_test_1(void) {
 _TPtr<int> q = 0;
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  _TArray_ptr<int> s : count(5) = r; //expected-error {{initializing '_TArray_ptr<int>' with an expression of incompatible type 'int _Checked[10]}}

  printf("Printable0\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, count(3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable1\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r+3, count(3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable2\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, bounds(s, s+3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable3\n");

  _TNt_array_ptr<const char> p : count(2) =
    _Tainted_Dynamic_bounds_cast<_TNt_array_ptr<const char>>("abcdef", count(2)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable4\n");

  printf("Expected Success");
  return;
}

// dynamic_check(base == NULL || ...)
// dynamic_check(base == NULL || ...)
void passing_test_2(void) {
 _TPtr<int> q = 0;
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  _TArray_ptr<int> s : count(5) = r; //expected-error {{initializing '_TArray_ptr<int>' with an expression of incompatible type 'int _Checked[10]'}}

  // constant folded
  q = _Tainted_Dynamic_bounds_cast<ptr<int>>(NULL); //expected-error{{use of undeclared identifier 'NULL'}}
  printf("Printable0\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(NULL, bounds(r, r+5));//expected-error{{use of undeclared identifier 'NULL'}}
  printf("Printable1\n");

  s = NULL;//expected-error{{use of undeclared identifier 'NULL'}}
  q = _Tainted_Dynamic_bounds_cast<_TPtr<int>>(s);
  printf("Printable2\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(s, bounds(r, r+5));
  printf("Printable3\n");

  printf("Expected Success");
  return;
}

// Test dynamic checks involving conversions to void pointers.
void passing_test_3(void) {
  int i = 10;
 _TPtr<int> pi = &i; //expected-error {{initializing '_TPtr<int>' with an expression of incompatible type 'int *'}}
 _TPtr<void> pv = pi;
  void *unchecked_pv = 0;
 _TPtr<void> p1 = _Tainted_Dynamic_bounds_cast<_TPtr<void>>(pi);
  printf("Passed p1");
 _TPtr<void> p2 = _Tainted_Dynamic_bounds_cast<_TPtr<void>>(pv);
  printf("Passed p2");
  unchecked_pv = _Tainted_Dynamic_bounds_cast<void *>(pi); //expected-error {{expected _TPtr}}
  printf("Passed unchecked_pv");
 _TPtr<void> p3 = _Tainted_Assume_bounds_cast<_TPtr<void>>(unchecked_pv); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Passed p3");
  void *p4 = _Assume_bounds_cast<void *>(unchecked_pv);
  printf("Passed p4");
  printf("Expected Success");
  return;
}

// dynamic_check(r != NULL) && dynamic_check(r <= r && r+15 <= r+10) -> FAIL
void failing_test_1(void) {
 _TPtr<int> q = 0;
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, count(15)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}

  printf("Unprintable\n");

  printf("Unexpected Success");
  return;
}

// dynamic_check((r+8) != NULL) && dynamic_check(r <= r+8 && (r+8+3) <= r+10) -> FAIL
void failing_test_2(void) {
 _TPtr<int> q = 0;
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r+8, count(3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}

  printf("Unprintable\n");

  printf("Unexpected Success");
  return;
}

// dynamic_check(r != NULL) && dynamic_check(r <= s && s+3 <= r+10) -> FAIL
void failing_test_3(void) {
 _TPtr<int> q = 0;
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  _TArray_ptr<int> s : count(5) = r; //expected-error {{initializing '_TArray_ptr<int>' with an expression of incompatible type 'int _Checked[10]'}}

  q = _Tainted_Dynamic_bounds_cast<_TPtr<int>>(r); // expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable0\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, count(5)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable1\n");

  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, bounds(s, s+3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  printf("Printable2\n");

  s = 0;
  q = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, bounds(s, s+3)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}

  printf("Unprintable\n");

  printf("Unexpected Success");
  return;
}

// bounds_cast insert dynamic_check(r <= r && (r+5) <= r+10) -> OK;
// dereference insert dynamic_check(s <= s+5 && (s+5) < s+3) -> FAIL
// k = 5
void failing_test_4(int k) {
  int r _Checked[10] = {0,1,2,3,4,5,6,7,8,9};
  _TArray_ptr<int> s : count(3) = _Tainted_Dynamic_bounds_cast<_TArray_ptr<int>>(r, count(5)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}

  printf("Printable1\n");
  printf("Unprintable2: %d\n", *(s+k));

  printf("Unexpected Success");
  return;
}

// TEst dynamic checks involving possibly failig conversions to_TPtr<int>.
void failing_test_5(_TArray_ptr<char> pc : count(len), unsigned len) {
 _TPtr<int> pi = _Tainted_Dynamic_bounds_cast<_TPtr<int>>(pc);
  if (len < sizeof(int))
    printf("Unexpected Success");
  else if (*pi == 0)
    printf("Successful pointer conversion");
  return;
}

// Test dynamic checks involving possibly failing conversions to_TPtr<void>.
void failing_test_6(_TArray_ptr<char> pc : count(len), unsigned len) {
 _TPtr<void> pv = _Tainted_Dynamic_bounds_cast<_TPtr<void>>(pc);
  if (len == 0)
    printf("Unexpected Success");
  else if (pv != 0)
    printf("Successful conversion to_TPtr<void>");
  return;
}

// Test dynamic checks involving possibly failing conversions to void *.
void failing_test_7(_TArray_ptr<char> pc : count(len), unsigned len) {
 void *pv = _Tainted_Dynamic_bounds_cast<void *>(pc); //expected-error {{expected _TPtr}}
  if (len == 0)
    printf("Unexpected Success");
  else if (pv != 0)
    printf("Successful conversion to void *");
  return;
}

// Test dynamic checks involving possibly failing conversions from
// string literals.
void failing_test_8(unsigned len) {
 _TNt_array_ptr<const char> p : count(len) =
   _Tainted_Dynamic_bounds_cast<_TNt_array_ptr<const char>>("123456", count(len)); //expected-error {{Only Tainted-pointers allowed as part of bounds expression for Tainted Cast Operations}}
  if (len > 6)
    printf("Unexpected Success");
  else if ((len == 0 && p != 0) || *p == '1')
    printf("Successful conversion to _TNt_array_ptr<const char>");
  return;
}

