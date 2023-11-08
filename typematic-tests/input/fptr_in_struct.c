#include <stdio.h>

typedef struct FunctionalStruct {
    void (*print)(const char*);
} FunctionalStruct;

void say_hello(const char* name) {
    printf("Hello, %s!\n", name);
}

int main() {
    FunctionalStruct fs;
    fs.print = say_hello;
    char* text = "world";
    fs.print(text); // Should print "Hello, World!"
    _TPtr<char> t_text = text;
    return 0;
}

