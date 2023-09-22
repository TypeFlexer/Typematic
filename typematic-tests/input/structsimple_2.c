typedef struct anothersimp {
	char* arg4;
	int* arg5;
	struct anothersimp* simp3;
} an;
typedef struct simple{
	int arg1;
	int* arg2;
	char* arg3;
	struct simple* simp1;
	an* simp2;
} si;

int main(){
	si s;
	si* ts;
	_TPtr<int> targ2;
	s.arg2 = targ2;
	an a;
	a.arg5 = s.arg2;
	return 0;
}
