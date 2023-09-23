#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

_Tainted _TPtr<int> increment(_TPtr<int> tainted_ptr) {
    (*tainted_ptr)++;
    return tainted_ptr;
}

_Tainted int main() {
    _TPtr<int> seed_tainted_ptr = t_malloc(sizeof(int));
    *seed_tainted_ptr = 10;

    printf("Value before increment: %d\n", *seed_tainted_ptr);

    seed_tainted_ptr = increment(seed_tainted_ptr);

    printf("Value after increment: %d\n", *seed_tainted_ptr);

    t_free<int>(seed_tainted_ptr);
    return 0;
}