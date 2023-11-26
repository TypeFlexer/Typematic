#include <stdio.h>

typedef _Ptr<_Ptr<int> (_Ptr<int> ptr)> another_name_for_callback; // Callback accepts and returns int*

_Ptr<int> my_function(_Ptr<int> ptr) _Checked {
_Unchecked { printf("Callback invoked, value: %d\n", *ptr); };
return ptr; // Return is a regular int pointer, not tainted
}

void trigger_callback(another_name_for_callback cb, _Ptr<int> ptr) _Checked {
_Ptr<int> result = cb(ptr);
}

int main() {
    int a = 2;
    int* seed = __Marshall__T2C(&a); // This is a regular pointer, not yet tainted

    _TPtr<int> seed_t = seed;

    trigger_callback(my_function, _Assume_bounds_cast<_Ptr<int>>(seed));

    return 0;
}