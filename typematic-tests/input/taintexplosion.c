#include <stdio.h>

// Simulating _TPtr<T> for tainted pointer

int main() {
    // Original pointer
    _TPtr<int> tainted_ptr = (_TPtr<int>)t_malloc(10*sizeof(int));

    // Aliases of the original pointer
    int *alias1 = tainted_ptr;
    int *alias2 = alias1;
    int *alias3 = alias2;
    int *alias4 = alias3;
    int* alias5 = alias2;
    int* someother_1;
    int* someother_2;
    alias5 = someother_1;
    alias5 = someother_2;


    return 0;
}

