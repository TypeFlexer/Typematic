#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib_tainted.h>

typedef struct anothersimp {
    _TPtr<char> arg4;
    _Ptr<int> arg5;
    _Ptr<struct anothersimp> simp3;
} an;

typedef struct simple {
    int arg1;
    _Ptr<int> arg2;
    _TPtr<char> arg3;
    _Ptr<struct simple> simp1;
    _Ptr<an> simp2;
} si;

typedef struct asimple {
    int arg1;
    _Ptr<int> arg2;
    _Ptr<char> arg3;
    _Ptr<struct simple> simp1;
    _Ptr<an> simp2;
} asi;


int main(){
    _TPtr<si> ts = t_malloc<si>(sizeof(si));
    _Array_ptr<char> name : byte_count(17) = "Arunkumar Bhattar";

    _Ptr<int> targ2 = (_Ptr<int>)malloc<int>(10 * sizeof(int));
    for (int i = 0; i < 10; i++) _Checked {
        targ2[i] = i;
    }

    ts->arg2 = targ2;
    ts->arg1 = 100;
    ts->arg3 = t_malloc<char>((strlen(name) + 1) * sizeof(char));
    strncpy(ts->arg3, name, strlen(name) + 1); // +1 to include the null terminator

    _TPtr<an> as = t_malloc<an>(sizeof(an));
    as->arg4 = ts->arg3;
    as->arg5 = ts->arg2;

    asi *ats = (asi*)ts;
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
    free<int>(targ2);
    t_free<char>(ts->arg3);
    t_free<si>(ts);
    t_free<an>(as);

    return 0;
}