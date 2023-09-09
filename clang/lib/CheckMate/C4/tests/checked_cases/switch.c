#include <stdio.h>

void foo(char c) {

  switch (c){
  case '\0':
                break;
            default: /*HACK! needs to be here for this to work*/
                _Unchecked {
printf("%s","hello");
                }
  }
}

