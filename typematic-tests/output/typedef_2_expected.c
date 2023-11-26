#include <stdio.h>

typedef _Ptr<_TPtr<int> (_TPtr<int> ptr)> another_name_for_callback; // Callback accepts and returns int*

_Tainted _TPtr<int> my_function(_TPtr<int> ptr) {
printf("Callback invoked, value: %d\n", *ptr);
return ptr; // Return is a regular int pointer, not tainted
}

_Tainted void trigger_callback(another_name_for_callback cb, _TPtr<int> ptr) {
_TPtr<int> result = cb(ptr);
}

_Tainted int main() {
    int a = 2;
    _TPtr<int> seed = __Marshall__T2C(&a); // This is a regular pointer, not yet tainted

    _TPtr<int> seed_t = seed;

    trigger_callback(my_function, seed);

    return 0;
}