#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib_tainted.h>

typedef struct anothersimp {
        char* arg4;
        int* arg5;
        struct anothersimp* simp3;
} an;

typedef struct simple {
        int arg1;
        int* arg2;
        char* arg3;
        struct simple* simp1;
        an* simp2;
} si;

int main(){
        _TPtr<si> ts = t_malloc(sizeof(si));
        char* name = "Arunkumar Bhattar";

        int* targ2 = (int*)malloc(10 * sizeof(int));
        for (int i = 0; i < 10; i++) {
                targ2[i] = i;
        }

        ts->arg2 = targ2;
        ts->arg1 = 100;
        ts->arg3 = t_malloc((strlen(name) + 1) * sizeof(char));
        strncpy(ts->arg3, name, strlen(name) + 1); // +1 to include the null terminator

        an* as = t_malloc(sizeof(an));
        as->arg4 = ts->arg3;
        as->arg5 = ts->arg2;

        // Print statements
        printf("si structure values:\n");
        printf("arg1: %d\n", ts->arg1);
        printf("arg3: %s\n", ts->arg3);

        printf("arg2 array values:\n");
        for(int i = 0; i < 10; i++) {
            printf("arg2[%d]: %d\n", i, ts->arg2[i]);
        }

        printf("anothersimp structure values:\n");
        printf("arg4: %s\n", as->arg4);

        printf("arg5 array values:\n");
        for(int i = 0; i < 10; i++) {
            printf("arg5[%d]: %d\n", i, as->arg5[i]);
        }

        // Free allocated memory
        free(targ2);
        free(ts->arg3);
        free(ts);
        free(as);

        return 0;
}
