#include <stdio.h>

typedef int* (*another_name_for_callback)(int* ptr); // Callback accepts and returns int*

int* my_function(int* ptr) {
    printf("Callback invoked, value: %d\n", *ptr);
    return ptr; // Return is a regular int pointer, not tainted
}

void trigger_callback(another_name_for_callback cb, int* ptr) {
    int* result = cb(ptr);
}

int main() {
    int a = 2;
    int* seed = &a; // This is a regular pointer, not yet tainted

    _TPtr<int> seed_t = seed;

    trigger_callback(my_function, seed_t);

    return 0;
}
