int main(){
    char* name1 = "Alice";
    char* name2 = "Bob";
    _TPtr<char> name_tptr1 = name1;
    _TPtr<char> name_tptr2 = name2;

    int scores1[5] = {90, 80, 70, 60, 50};
    int scores2[5] = {45, 55, 65, 75, 85};
    _TPtr<int> tainted_scores1 = scores1;
    _TPtr<int> tainted_scores2 = scores2;

    _TPtr<char> alias_name_tptr = name_tptr1;  // Aliasing of pointer
    _TPtr<int> alias_scores_tptr = tainted_scores1;  // Aliasing of pointer

    name_tptr1 = name_tptr2;  // Contravariant propagation
    tainted_scores1 = tainted_scores2;  // Contravariant propagation

    char* alias_name = name1;  // Aliasing of pointer
    int* alias_scores = scores1;  // Aliasing of pointer

    alias_name_tptr = name_tptr2;  // Covariant propagation
    alias_scores_tptr = tainted_scores2;  // Covariant propagation

    return 0;
}

