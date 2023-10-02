#include <stdio.h>

_Tainted int main() {
    float temperatures[2][4] = {
        {72.5, 75.0, 68.9, 70.2},
        {65.5, 68.0, 64.8, 67.1}
    };

    _TPtr<_TPtr<float>> val = __ConstantNonLinearbufferToTainted__(temperatures, 2, 4, sizeof(float));

    for(int i = 0; i < 2; ++i) _Checked {
        for(int j = 0; j < 4; ++j) _Unchecked {
            printf("%.1f ", temperatures[i][j]);
        }
        _Unchecked { printf("\n"); };
    }

    return 0;
}

