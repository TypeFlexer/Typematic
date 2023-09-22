#include <stdio.h>
#include <stdlib_tainted.h>
#include <string.h>

int main() {
    _TPtr<char> ptr = t_malloc<char>(20);
    strcpy(ptr, "Hello World");
    ptr += 6;
    printf("%s\n", ptr);
    ptr -= 6;
    t_free(ptr);
    return 0;
}

