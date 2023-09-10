#include <stdio.h>
#include <stdlib_tainted.h>
#include <string.h>

int main() {
    _TPtr<char> str1 = t_malloc<char>(50);
    _TPtr<char> str2 = t_malloc<char>(50);
    strcpy(str1, "Hello ");
    strcpy(str2, "World!");
    strcat(str1, str2);
    printf("%s\n", str1);
    t_free(str1);
    t_free(str2);
    return 0;
}

