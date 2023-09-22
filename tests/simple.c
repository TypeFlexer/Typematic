#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib_tainted.h>
int main()
{
	_TPtr<char> a = NULL;
	a = t_malloc<char>(100);
	strcpy(a, "hiarun!");
	printf("%s",a);
	return 0;
}
