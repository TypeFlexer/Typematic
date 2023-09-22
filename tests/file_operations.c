#include <stdio.h>
#include <stdlib_tainted.h>
#include <stdio_tainted.h>
#include <string.h>

int main() {
    //files can only be open in checked region
    _Ptr<FILE> file = fopen("example.txt", "w");
    if (file == NULL) {
        perror("Could not open file");
        return 1;
    }

    _TPtr<char> str = t_malloc<char>(100);
    sprintf(str, "Hello World with Tainted Pointers\n");

    fwrite(str, 1, strlen(str), file);

    fclose(file);
    return 0;
}

