#include <stdio.h>
#include <stdlib_tainted.h>

int main() {
    _TPtr<int> ptr = t_malloc<int>(1);
    *ptr = 42;
    printf("Value: %d\n", *ptr);
    t_free(ptr);
    return 0;
}

