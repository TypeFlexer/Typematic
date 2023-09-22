#include <stdio.h>
#include <stdlib_tainted.h>
#include <string.h>

int main() {
    _TPtr<char> input = t_malloc<char>(100);
    printf("Please enter a string: ");
    fgets(input, 100, stdin);

    _TPtr<char> reversed = t_malloc<char>(100);
    int len = strlen(input);
    for(int i = 0; i < len; ++i) {
        reversed[len-i-1] = input[i];
    }
    reversed[len] = '\0';

    printf("Reversed string: %s\n", reversed);

    t_free(input);
    t_free(reversed);
    return 0;
}

