#include <stdio.h>

typedef int* (*callback_t)(int* ptr); // Callback accepts and returns int*
typedef callback_t another_name_for_callback;

int* my_function(int* ptr) {
    printf("Callback invoked, value: %d\n", *ptr);
    return ptr; // Return is a regular int pointer, not tainted
}

void trigger_callback(another_name_for_callback cb, int* ptr) {
    int* result = cb(ptr);
    // ... use result ...
}

int main() {
    int x = 10;
    _TPtr<int> seed = &x; // This is a regular pointer, not yet tainted

    // Here we would assume the tool somehow taints the 'seed' and thus influences the callback.
    // For example, your tool could mark 'seed' as tainted if it's used in a certain context
    // where the taint could propagate.
    // This could be simulated or indicated with a comment or macro, for example:
    // _TPtr<int> tainted_seed = TAINT(seed); // Pseudo-operation to taint the seed

    trigger_callback(my_function, seed);

    return 0;
}
