#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

typedef Tstruct Dataval{
    _TPtr<double> taintedValue;
    double normalValue;
} Data;


_Tainted void displayData(_TPtr<Data> data) {
    printf("Tainted Value: %lf\n", *(data->taintedValue));
    printf("Normal Value: %lf\n", data->normalValue);
}

int main() {
    Data data = {};
    data.taintedValue = t_malloc<double>(sizeof(double));
    data.normalValue = 5.5;

    if (!data.taintedValue) _Checked {
        _Unchecked { printf("Memory allocation failed.\n"); };
        exit(1);
    }

    *(data.taintedValue) = 10.5;

    displayData(&data);

    t_free<double>(data.taintedValue);

    return 0;
}
