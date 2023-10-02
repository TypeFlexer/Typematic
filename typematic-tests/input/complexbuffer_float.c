#include <stdio.h>

int main() {
    float temperatures[2][4] = {
        {72.5, 75.0, 68.9, 70.2},
        {65.5, 68.0, 64.8, 67.1}
    };

    _TPtr<_TPtr<float>> val = temperatures;

    for(int i = 0; i < 2; ++i) {
        for(int j = 0; j < 4; ++j) {
            printf("%.1f ", temperatures[i][j]);
        }
        printf("\n");
    }

    return 0;
}

