// Function to add two integers
_Ptr<int (_TPtr<int>, _TPtr<int>)> o_fptr _Checked[2] = {((void *)0)};

_Tainted int add(_TPtr<int> a, _TPtr<int> b) {
return *a + *b;
}

// Function to multiply two integers
_Tainted int multiply(_TPtr<int> a, _TPtr<int> b) {
return *a * *b;
}

int main() {
    // Function pointers
    // Example usage
    _TPtr<int> x = ((void *)0);
    _TPtr<int> y = ((void *)0);
    _TPtr<int> a = ((void *)0);
    _TPtr<int> b = ((void *)0);
    _TPtr<int> c = ((void *)0);
    _TPtr<int> d = ((void *)0);
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