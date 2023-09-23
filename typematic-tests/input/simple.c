#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

int* simple (int* a, int* b)
{
        *b = *a + *b;
        return b;
}

int main() {
    // Allocate an int on the heap and set its value
    //_TPtr<int> heapInt = t_malloc(10*sizeof(int));
    _TPtr<int> heapInt;
    int* totally_untouched = malloc(10*sizeof(int));
    // Create several aliases for the heap-allocated int
    int* alias1 = heapInt;
    int* alias2 = alias1;
    //int* simpleVCal = (int*)malloc(1*sizeof(int));
    // Manipulate the heap-allocated int through its aliases
    alias1 = alias1 + 1;

    int* alias3 = simple(alias1, alias2);
    // Print the final value of the heap-allocated int
    printf("Final value: %d\n", *heapInt);

    free(heapInt);

    return 0;
}

