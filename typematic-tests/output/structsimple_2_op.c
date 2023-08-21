typedef struct anothersimp {
	_Ptr<char> arg4;
	_Ptr<int> arg5;
	_Ptr<struct anothersimp> simp3;
} an;
typedef struct simple{
	int arg1;
	_Ptr<int> arg2;
	_Ptr<char> arg3;
	_Ptr<struct simple> simp1;
	_Ptr<an> simp2;
} si;

int main()_Checked {
	si s = {};
	_Ptr<si> ts = ((void *)0);
	_TPtr<int> targ2;
	s.arg2 = targ2;
	return 0;
}
