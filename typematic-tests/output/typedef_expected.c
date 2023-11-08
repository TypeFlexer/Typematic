#include <stdio.h>

typedef _TPtr<int> (*callback_t )(_TPtr<int> ptr); // Callback accepts and returns int*
typedef _TPtr<int> (*another_name_for_callback )(_TPtr<int>);

_TPtr<int> my_function(_TPtr<int> ptr) {
    printf("Callback invoked, value: %d\n", *ptr);
    return ptr; // Return is a regular int pointer, not tainted
}

void trigger_callback(another_name_for_callback cb, _TPtr<int> ptr) {
    _TPtr<int> result = cb(ptr);
}

int main() {
    int a = 2;
    _TPtr<int> seed = __Marshall__T2C(a); // This is a regular pointer, not yet tainted

    _TPtr<int> seed_t = seed;

    trigger_callback(my_function, seed);

    return 0;
}