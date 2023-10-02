#include <stdio.h>

int main() {
    char sentences[3][256] = {
        "The quick brown fox jumps over the lazy dog.",
        "A journey of a thousand miles begins with a single step.",
        "To be, or not to be, that is the question."
    };

    _TPtr<_TPtr<char>> val = sentences;

    for(int i = 0; i < 3; ++i) {
        printf("%s\n", sentences[i]);
    }

    return 0;
}

