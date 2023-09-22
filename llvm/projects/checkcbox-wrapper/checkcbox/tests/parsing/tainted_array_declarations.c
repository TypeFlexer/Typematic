// Feature tests of parsing new CheckCBox Tainted type
//
// * Test that Tainted array types can be used as parameter types, local 
//   variable types, structure member types, in typedefs, and as arguments
//   to operators that take types.  In C, return types for functions
//   cannot be array types.
// * Test parsing of array types and function types combined with checked
//   array types.
//
// The following lines are for the LLVM test harness:
//
// RUN: %clang_cc1  -verify -verify-ignore-unexpected=warning -verify-ignore-unexpected=note %s

//
// parameter have new checked array types
//


extern void f1(_TPtr<int> a, int y) {
}

extern void f2(_TPtr<int> a _Checked[5], int y) {
}

extern void f3(const _TPtr<int> a _Checked[5], int y) {
}

extern void f4(_TPtr<int> a _Checked[][5], int y) {
}

extern void f5(_TPtr<int> a _Checked[5][5], int y) {
}

// not necessary, but syntactically legal
extern void f6(_TPtr<int> a _Checked[]_Checked[5], int y) {
}

// not necessary, but syntactically legal
extern void f7(_TPtr<int> a _Checked[5]_Checked[5], int y) {
}

//
// Null-terminated array versions
//

extern void f10(_TPtr<int> a _Nt_checked[], int y) {
}

extern void f11(_TPtr<int> a _Nt_checked[5], int y) {
}

extern void f12(const _TPtr<int> a _Nt_checked[5], int y) {
}

extern void f12a(_TPtr<const int> a _Nt_checked[5], int y) {
}

// Arrays of null-terminated checked arrays are allowed.
extern void f13(_TPtr<int> a _Checked[]_Nt_checked[5], int y) {
}

extern void f14(_TPtr<int> a _Checked[5]_Nt_checked[5], int y) {
}


//
// Second parameter is a new pointer type
//

extern void g1(int y, _TPtr<int> a _Checked[]) {
}

extern void g2(int y, _TPtr<int> a _Checked[5]) {
}

extern void g3(int y, _TPtr<const int> a _Checked[5]) {
}

extern void g4(int y, _TPtr<int> a _Checked[][5]) {
}

extern void g5(int y, _TPtr<int> a _Checked[5][5]) {
}

// not necessary, but syntactically legal
extern void g6(int y, int a _Checked[]_Checked[5]) {
}

// not necessary, but syntactically legal
extern void g7(int y, int a _Checked[5]_Checked[5]) {
}

//
// Null-terminated checked array versions.
//
extern void g10(int y, int a _Nt_checked[]) {
}

extern void g11(int y, int a _Nt_checked[5]) {
}

extern void g12(int y, const int a _Nt_checked[5]) {
}

// Arrays of null-terminated checked arrays are allowed.
extern void g13(int y, int a _Checked[]_Nt_checked[5]) {
}

extern void g14(int y, int a _Checked[5]_Nt_checked[5]) {
}

//
// Local variables with pointer types
//

extern void k1(int y)
{
    int arr1 _Checked[] = { 0, 1 , 2 };
    int arr2 _Checked[5];
    int arr3 _Checked[][5] = { { 1 }, {2 } };
    int arr4 _Checked[5][5];
    int arr5 _Nt_checked[] = { 0, 1, 2, 0 };
    int arr6 _Nt_checked[5];
}

extern void k2(int y)
{
    _TPtr<int> arr1 _Checked[] = {};
    _TPtr<int> arr2 _Checked[5] = {};
    _TPtr<int> arr3 _Checked[][5] = {};
    _TPtr<int> arr4 _Checked[5][5] = {};
    _TPtr<int> arr5 _Nt_checked[] = {0};
    _TPtr<int> arr6 _Nt_checked[5] = {0};
}


//
// Struct member with fixed-sized array types
//

struct Vector {
    int data _Checked[5];
    int len;
};

extern int Multiply(struct Vector vec1, struct Vector vec2) {
    if (vec1.len != vec2.len) {
       return 1;
    }
    for (int i = 0; i < vec1.len; i++) {
        vec1.data[i] *= vec2.data[i];
    }
    return 0;
}

extern int Multiply2(_TPtr<struct Vector> vec1p, _TPtr<struct Vector> vec2p) {
    if (vec1p->len != vec2p->len) {
       return 1;
    }
    for (int i = 0; i < vec1p->len; i++) {
        vec1p->data[i] *= vec2p->data[i];
    }
    return 0;
}

//
// Declaring checked arrays of function pointers
//

struct FixedLengthString {
  char str _Nt_checked[5];
};

int (*array_of_unchecked_ptr_to_func _Checked[10])(int x, int y);
int (*nullterm_array_of_unchecked_ptr_to_func _Nt_checked[10])(int x, int y);
extern int (*incomplete_array_of_unchecked_ptr_to_func _Checked[])(int x, int y);
extern int (*nullterm_incomplete_array_of_unchecked_ptr_to_func _Nt_checked[])(int x, int y);
_TPtr<int(int x, int  y)> array_of_ptr_to_func _Checked[10];
extern _TPtr<int(int x, int  y)> array_of_ptr_to_func _Checked[];
_TPtr<int(int x _Checked[10], int y)> aptr2 _Checked[10]; // expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TPtr<int(int x _Nt_checked[10], int y)> aptr3 _Nt_checked[10];// expected-error {{Tainted Pointers cannot point to Checked pointers}}

//
// Declaring pointers to arrays and arrays of pointers
//
int (*unchecked_ptr_to_array)_Checked[5];
_TPtr<int _Checked[5]> ptr_to_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TPtr<int _Nt_checked[5]> ptr_to_nullterm_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TArray_ptr<int _Checked[5]> _TArray_ptr_to_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TArray_ptr<int _Nt_checked[5]> _TArray_ptr_to_nullterm_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}

int(*unchecked_ptr_to_incomplete_array)_Checked[];
_TPtr<int _Checked[]> ptr_to_incomplete_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TPtr<int _Nt_checked[]> ptr_to_nullterm_incomplete_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TArray_ptr<int _Checked[]> _TArray_ptr_to_incomplete_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
_TArray_ptr<int _Nt_checked[]> _TArray_ptr_to_nullterm_incomplete_array;// expected-error {{Tainted Pointers cannot point to Checked pointers}}

// Declaring checked arrays of pointers
int *array_of_unchecked_ptrs _Checked[5];
int *nullterm_array_of_unchecked_ptrs _Nt_checked[5];
_TPtr<int> array_of_ptrs _Checked[5];
_TPtr<int> nullterm_array_of_ptrs _Nt_checked[5];
_TArray_ptr<int> array_of__TArray_ptrs _Checked[5];
_TArray_ptr<int> nullterm_array_of__TArray_ptrs _Nt_checked[5];
_TNt_array_ptr<int> array_of_nullterm_array_ptrs _Checked[5];
_TNt_array_ptr<int> nullterm_array_of_nullterm_array_ptrs _Nt_checked[5];

// Declare an unchecked pointer to checked arrays of pointers
int *(*uncheckedptr_to_array_of_unchecked_ptrs) _Checked[5];
_TPtr<int>(*unchecked_ptr_to_array_of_ptrs) _Checked[5];
_TArray_ptr<int>(*unchecked_ptr_to_array_of_array_ptrs) _Checked[5];
_TArray_ptr<int>(*unchecked_ptr_to_null_term_array_of_array_ptrs) _Nt_checked[5];

/*ALL OF THE BELOW MUST FAIL*/
// Declare ptr to checked arrays of pointers
_TPtr<int *_Checked[5]> ptr_to_array_of_unchecked_ptrs; // expected-error {{Tainted Pointer cannot point to Un-Checked pointers}}
_TPtr<_TPtr<int> _Checked[5]> ptr_to_array_of_ptrs; // expected-error {{Tainted Pointers cannot point to Checked pointers}} 
_TPtr<_TArray_ptr<int> _Checked[5]> ptr_to_array_of_array_ptrs;// expected-error {{Tainted Pointers cannot point to Checked pointers}} 
_TPtr<_TArray_ptr<int> _Nt_checked[5]> ptr_to_nullterm_array_of_array_ptrs; // expected-error {{Tainted Pointers cannot point to Checked pointers}} 
_TPtr<_TNt_array_ptr<int> _Nt_checked[5]> ptr_to_nullterm_array_of_nullterm_array_ptrs;// expected-error {{Tainted Pointers cannot point to Checked pointers}} 

// Declare ptr to a checked array of function pointers
_TPtr<int (*_Checked[5])(int x, int y)> ptr_to_array_of_unchecked_func_ptrs; //expected-error {{Tainted Pointer cannot point to Un-Checked pointers}}
_TPtr<_TPtr<int (int x, int y)>_Checked [5]> ptr_to_array_of_checked_func_ptrs;// expected-error {{Tainted Pointers cannot point to Checked pointers}} 
_TPtr<_TPtr<int (int x, int y)>_Nt_checked [5]> ptr_to_nullterm_array_of_checked_func_ptrs;// expected-error {{Tainted Pointers cannot point to Checked pointers}} 
// Make parameter and return types be ptrs too.
_TPtr<_TPtr<_TPtr<int>(_TPtr<int> x, _TPtr<int> y)>_Checked[5]>// expected-error {{Tainted Pointers cannot point to Checked pointers}}
ptr_to_array_of_checked_func_ptrs_with_ptr_parameters; 
_TPtr<_TPtr<_TPtr<int> (_TPtr<int> x, _TPtr<int> y)>_Nt_checked[5]>// expected-error {{Tainted Pointers cannot point to Checked pointers}}
  ptr_to_nullterm_array_of_checked_func_ptrs_with_ptr_parameters; 


//
// Typedefs that define tainted pointers pointing
// to checked and Unchecked pointer types
//
_TPtr<int> test1;
typedef _TPtr<int> arr_ty[5];
typedef _TPtr<int> incomplete_arr_ty[];
typedef _TPtr<int> checked_arr_ty _Checked[5];
typedef _TPtr<int> incomplete_checked_array_ty _Checked[];
typedef _TPtr<int> nullterm_checked_arr_ty _Nt_checked[5];
typedef _TPtr<int> nullterm_incomplete_checked_array_ty _Nt_checked[];
typedef _Ptr<_TPtr<int>_Checked[5]>* tmp;
//
// Operators that take types
//

void parse_operators_with_types(void) {
    int s1 = sizeof(int _Checked[10]);
    int s2 = sizeof(_TPtr<int _Checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s3 = sizeof(_TArray_ptr<int _Checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s4 = sizeof(_TPtr<int>_Checked[5]);
    int s44 = sizeof(_TPtr<int>*);
    int s45 = sizeof(_TPtr<_TPtr<int>*>*); //expected-error {{Tainted Pointer cannot point to Un-Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s5 = sizeof(_TArray_ptr<int> _Checked[5]);

    // null-terminated versions.
    int s6 = sizeof(int _Nt_checked[10]);
    int s7 = sizeof(_TPtr<int _Nt_checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s8 = sizeof(_TArray_ptr<int _Nt_checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s9 = sizeof(_TPtr<int> _Nt_checked[5]);
    int s10 = sizeof(_TArray_ptr<int> _Nt_checked[5]);

    int s20 = _Alignof(int _Checked[10]);
    int s21 = _Alignof(_TPtr<int _Checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s22 = _Alignof(_TArray_ptr<int _Checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s23 = _Alignof(_TPtr<int> _Checked[5]);
    int s24 = _Alignof(_TArray_ptr<int>_Checked[5]);
    int s25 = _Alignof(_TNt_array_ptr<int>_Checked[5]);

    // null-terminated versions.
    int s26 = _Alignof(int _Nt_checked[10]);
    int s27 = _Alignof(_TPtr<int _Nt_checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s28 = _Alignof(_TArray_ptr<int _Nt_checked[5]>);// expected-error {{Tainted Pointers cannot point to Checked pointers}} expected-error {{type name requires a specifier or qualifier}} expected-error {{expected ')'}}
    int s29 = _Alignof(_TPtr<int> _Nt_checked[5]);
    int s30 = _Alignof(_TArray_ptr<int> _Nt_checked[5]);
    int s31 = _Alignof(_TNt_array_ptr<int> _Nt_checked[5]);

    // Test parsing of some cast operations that should pass checking
    // of bounds declarations
    int x = 0;
    int arr _Checked[5];
    _TPtr<int> px = (_TPtr<int>) &x; //expected-error {{Untainted Pointer 'int *' cannot be cast to Tainted Pointer '_TPtr<int>' Address pointed by 'int *' should not be exposed to Unchecked Region}}
    _TArray_ptr<int> pax = (_TArray_ptr<int>) &x; //expected-error {{Untainted Pointer 'int *' cannot be cast to Tainted Pointer '_TArray_ptr<int>' Address pointed by 'int *' should not be exposed to Unchecked Region}}
     pax = arr; //expected-error {{assigning to '_TArray_ptr<int>' from incompatible type 'int _Checked[5]'}}

    _TPtr<int _Checked[5]> parr = 0;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
    parr = &arr; //expected-error {{use of undeclared identifier 'parr'; did you mean 'arr'?}} expected-error {{array type 'int _Checked[5]' is not assignable}}
    parr = (int (*)_Checked[5]) &arr; //expected-error {{use of undeclared identifier 'parr'; did you mean 'arr'?}} expected-error {{array type 'int _Checked[5]' is not assignable}}
    parr = (int (*)_Checked[5]) ((int (*)_Checked[]) &arr); //expected-error {{use of undeclared identifier 'parr'; did you mean 'arr'?}} expected-error {{array type 'int _Checked[5]' is not assignable}}
    // ptr to function type
    _TPtr<int (int x, int y)> pfunc_not_allowed = (_TPtr<int (int x, int y)>) 0; //expected-error{{Untainted Pointer 'int' cannot be cast to Tainted Pointer '_TPtr<int (int, int)>' Address pointed by 'int' should not be exposed to Unchecked Region}}
    _TPtr<int> ppi = 0;
    _TPtr<_TPtr<int> (int x, int y)> pfunc_allowed = (_TPtr<_TPtr<int> (int x, int y)>)ppi ;
    _TPtr<_TPtr<int (int x, int y)>_Checked [5]> ptr_to_pfunc_arr = (_TPtr<_TPtr<int (int x, int y)>_Checked[5]>) 0;// expected-error {{Tainted Pointers cannot point to Checked pointers}}
}

