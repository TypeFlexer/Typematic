#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

_Tainted _TPtr<int> simple (_TPtr<int> a, _TPtr<int> b)
{
	*b = *a + *b;
	return b;
}
int main() {
    // Allocate an int on the heap and set its value
    int* heapInt = malloc(10*sizeof(int));
    // Create several aliases for the heap-allocated int
    int* alias1 = heapInt;
    int* alias2 = alias1;
    // Manipulate the heap-allocated int through its aliases
    alias1 = alias1 + 1;

    int* alias3 = simple(alias1, alias2);
    // Print the final value of the heap-allocated int
    printf("Final value: %d\n", *heapInt);

    // Free the heap-allocated memory
    free(heapInt);

    return 0;
}

