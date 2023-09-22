#include <stdlib_tainted.h>
#include <stdio.h>

typedef void (*tainted_arithmetic_func)(_TPtr<int>, _TPtr<int>, _TPtr<int>);

void perform_tainted_arithmetic(tainted_arithmetic_func op, _TPtr<int> a, _TPtr<int> b, _TPtr<int> result) {
    op(a, b, result);
}

void my_tainted_add(_TPtr<int> a, _TPtr<int> b, _TPtr<int> result) {
	*result = *a + *b;
	printf("printing %d + %d = %d\n", *a, *b, *result);
}

void my_tainted_subtract(_TPtr<int> a, _TPtr<int> b, _TPtr<int> result) {
	*result = *a - *b;
	printf("printing %d - %d = %d\n", *a, *b, *result);
}

int main() {
    _TPtr<int> a = t_malloc<int>(1*sizeof(int));// ... obtain a tainted pointer
    _TPtr<int> b = t_malloc<int>(1*sizeof(int));// ... obtain another tainted pointer
    _TPtr<int> result = t_malloc<int>(1*sizeof(int));// ... obtain a tainted pointer for the result
	
    *a= 124;
    *b =1913;
    perform_tainted_arithmetic(my_tainted_add, a, b, result);
    perform_tainted_arithmetic(my_tainted_subtract, a, b, result);

    return 0;
}

