#include <stdio.h>

typedef Tstruct FunctionalStruct {
        void (*print )(_TPtr<const char>);
} FunctionalStruct;

void say_hello(_TPtr<const char> name) {
    printf("Hello, %s!\n", name);
}

int main() {
    FunctionalStruct fs = {};
    fs.print = say_hello;
    _TPtr<char> text = "world";
    fs.print(text); // Should print "Hello, World!"
    _TPtr<char> t_text = text;
    return 0;
}