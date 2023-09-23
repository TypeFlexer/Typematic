#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

_Tainted _TNt_array_ptr<int> simple(_TPtr<int> a, _TNt_array_ptr<int> b)
{
        *b = *a + *b;
        return b;
}

int main() {
    // Allocate an int on the heap and set its value
    //_TPtr<int> heapInt = t_malloc(10*sizeof(int));
    _TPtr<int> heapInt;
    _Ptr<int> totally_untouched = malloc<int>(10*sizeof(int));
    // Create several aliases for the heap-allocated int
    _TNt_array_ptr<int> alias1 = heapInt;
    _TNt_array_ptr<int> alias2 = alias1;
    //int* simpleVCal = (int*)malloc(1*sizeof(int));
    // Manipulate the heap-allocated int through its aliases
    alias1 = alias1 + 1;

    _TNt_array_ptr<int> alias3 = simple(alias1, alias2);
    // Print the final value of the heap-allocated int
    printf("Final value: %d\n", *heapInt);

    t_free<int>(heapInt);

    return 0;
}