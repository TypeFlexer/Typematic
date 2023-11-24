#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib_tainted.h>

typedef Tstruct anothersimp {
        _TPtr<char> arg4;
        _TPtr<int> arg5;
        _TPtr<Tstruct anothersimp> simp3;
} an;

typedef Tstruct simple {
        int arg1;
        _TPtr<int> arg2;
        _TPtr<char> arg3;
        _TPtr<Tstruct simple> simp1;
        _TPtr<an> simp2;
} si;

int main(){
    _TPtr<si> ts = t_malloc<si>(sizeof(si));
    _Ptr<char> name = "Arunkumar Bhattar";

    _TPtr<int> targ2 = (_TPtr<int>)malloc<int>(10 * sizeof(int));
    for (int i = 0; i < 10; i++) _Checked {
        targ2[i] = i;
    }

    ts->arg2 = targ2;
    ts->arg1 = 100;
    ts->arg3 = t_malloc<char>((strlen(name) + 1) * sizeof(char));
    strncpy(ts->arg3, name, strlen(name) + 1); // +1 to include the null terminator

    _TArray_ptr<an> as = t_malloc<an>(sizeof(an));
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
    t_free<int>(targ2);
    t_free<char>(ts->arg3);
    t_free<si>(ts);
    t_free<an>(as);

    return 0;
}