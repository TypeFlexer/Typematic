typedef Tstruct anothersimp {
		_TPtr<char> arg4;
		_TPtr<int> arg5;
		_TPtr<Tstruct anothersimp> simp3;
} an;
typedef Tstruct simple{
		int arg1;
		_TPtr<int> arg2;
		_TPtr<char> arg3;
		_TPtr<Tstruct simple> simp1;
		_TPtr<an> simp2;
} si;

int main()_Checked {
si s = {};
_TPtr<si> ts;
_TPtr<int> targ2;
s.arg2 = targ2;
return 0;
}