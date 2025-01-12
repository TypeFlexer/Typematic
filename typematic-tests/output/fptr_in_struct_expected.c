#include <stdio.h>

typedef struct FunctionalStruct {
    _Ptr<void (_TPtr<const char>)> print;
} FunctionalStruct;

void say_hello(_TPtr<const char> name) {
    printf("Hello, %s!\n", name);
}

_Tainted int main() {
    FunctionalStruct fs = {};
    fs.print = say_hello;
    _TPtr<char> text = __ConstantStringToTainted__("world", strlen("world"));
    fs.print(text); // Should print "Hello, World!"
    _TPtr<char> t_text = text;
    return 0;
}