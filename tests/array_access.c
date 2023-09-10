#include <stdio.h>
#include <stdlib_tainted.h>

int main() {
    _TPtr<int> arr = t_malloc<int>(5);
    for(int i = 0; i < 5; ++i) {
        arr[i] = i * i;
    }
    for(int i = 0; i < 5; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    t_free(arr);
    return 0;
}

