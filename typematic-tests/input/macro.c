#include <stdio.h>

#define INCREMENT(ptr, type) (type*)((char*)(ptr) + sizeof(type))
#define CAST_PTR(ptr, new_type) ((new_type)(ptr))
#define INDEX(arr, index, type) &((type*)(arr))[(index)]

int main() {
    int a[5] = {0, 1, 2, 3, 4};
    int *ptr = a;
    ptr = INCREMENT(ptr, int);
    printf("%d\n", *ptr); // Should print 1
    _TPtr<char> cptr = CAST_PTR(ptr, char*);
    printf("%c\n", *cptr); // Undefined behavior: interpreting int as char
    int *iptr = INDEX(a, 3, int);
    printf("%d\n", *iptr); // Should print 3
    return 0;
}

