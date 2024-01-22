// Function to add two integers
int (*o_fptr[2])(int*, int*);

_Tainted int add(int* a, int* b) {
    return *a + *b;
}

// Function to multiply two integers
int multiply(int* a, int* b) {
    return *a * *b;
}

int main() {
    // Function pointers
    // Example usage
    int* x;
    int* y;
    int* a;
    int* b;
    int* c;
    int* d;
    x = malloc(4);
    y = malloc(4);

    // Using the add function
    o_fptr[0] = add;
    o_fptr[1] = multiply;

    int result1 = o_fptr[0](x, y);

    // Using the multiply function

    int result2 = o_fptr[1](x, x);

    int result3 = o_fptr[1](a,b);
    int result4 = o_fptr[0](c,d);
    // Since we're not including stdio.h, we can't use printf
    // Normally, you'd output the results here

    return 0;
}