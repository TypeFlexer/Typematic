//#include <stdio.h>
//#include <stdlib.h>
//#include <stdlib_tainted.h>

_TPtr<int> t_malloc(long size);

_Tainted _TPtr<int> increment(_TPtr<int> tainted_ptr) {
(*tainted_ptr)++;
return tainted_ptr;
}

_Tainted int main() {
    _TPtr<int> seed_tainted_ptr = t_malloc(sizeof(int));
    //int* tptr = seed_tainted_ptr;
    *seed_tainted_ptr = 10;

    printf(((const char *)"Value before increment: %d\n"), *seed_tainted_ptr);

    seed_tainted_ptr = increment(seed_tainted_ptr);

    printf(((const char *)"Value after increment: %d\n"), *seed_tainted_ptr);

    t_free(seed_tainted_ptr);
    return 0;
}