int main()_Checked {
    _TPtr<char> name1 = __ConstantStringToTainted__("Alice", strlen("Alice"));
    _TPtr<char> name2 = __ConstantStringToTainted__("Bob", strlen("Bob"));
    _TPtr<char> name_tptr1 = name1;
    _TPtr<char> name_tptr2 = name2;

    int scores1 _Checked[5] = {90, 80, 70, 60, 50};
    int scores2 _Checked[5] = {45, 55, 65, 75, 85};
    _TPtr<int> tainted_scores1 = __ConstantArrayToTainted__(scores1, 5);
    _TPtr<int> tainted_scores2 = __ConstantArrayToTainted__(scores2, 5);

    _TPtr<char> alias_name_tptr = name_tptr1;  // Aliasing of pointer
    _TPtr<int> alias_scores_tptr = tainted_scores1;  // Aliasing of pointer

    name_tptr1 = name_tptr2;  // Contravariant propagation
    tainted_scores1 = tainted_scores2;  // Contravariant propagation

    _TNt_array_ptr<char> alias_name = name1;  // Aliasing of pointer
    _TArray_ptr<int> alias_scores = __ConstantArrayToTainted__(scores1, 5);  // Aliasing of pointer

    alias_name_tptr = name_tptr2;  // Covariant propagation
    alias_scores_tptr = tainted_scores2;  // Covariant propagation

    return 0;
}
