#include <stdio.h>

int main() _Checked {
    char sentences _Checked[3] _Checked[256] = {
        "The quick brown fox jumps over the lazy dog.",
        "A journey of a thousand miles begins with a single step.",
        "To be, or not to be, that is the question."
    };

    _TPtr<_TPtr<char>> val = __ConstantNonLinearbufferToTainted__(sentences, 3, 256, sizeof(char));

    for(int i = 0; i < 3; ++i) _Unchecked {
        printf("%s\n", sentences[i]);
    }

    return 0;
}
