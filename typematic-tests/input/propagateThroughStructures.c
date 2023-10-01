#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

typedef struct Dataval{
    _TPtr<double> taintedValue;
    double normalValue;
} Data;


void displayData(Data *data) {
    printf("Tainted Value: %lf\n", *(data->taintedValue));
    printf("Normal Value: %lf\n", data->normalValue);
}

int main() {
    Data data;
    data.taintedValue = malloc(sizeof(double));
    data.normalValue = 5.5;

    if (!data.taintedValue) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    *(data.taintedValue) = 10.5;
    
    displayData(&data);

    free(data.taintedValue);

    return 0;
}

