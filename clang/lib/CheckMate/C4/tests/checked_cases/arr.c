
int foo(_Ptr<int> x) { return *x + 1; }

int main(int argc, _Array_ptr<_Nt_array_ptr<char>> argv : count(argc)) {
  int arr _Checked[5] = {1, 2, 3, 4, 5};
  int arr2 _Nt_checked[5] = {1, 2, 3, 4, 5, 0};
  return foo(&argc);
}
