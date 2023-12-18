#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

// Function prototypes
double add(_TArray_ptr<double> a, _Ptr<double> b);
double subtract(_TArray_ptr<double> a, _Ptr<double> b);
double multiply(_TArray_ptr<double> a, _Ptr<double> b);
double divide(_TArray_ptr<double> a, _Ptr<double> b);
void displayMenu();
double executeOperation(_Ptr<double (_TArray_ptr<double>, _Ptr<double>)> operation, _TArray_ptr<double> a, _Ptr<double> b);

int main() {
    int choice;
    _TPtr<double> x = t_malloc<double>(sizeof(double));
    _Ptr<double> y = malloc<double>(sizeof(double));
    _Ptr<double> result = malloc<double>(sizeof(double));
    _Ptr<double (_TArray_ptr<double>, _Ptr<double>)> operation = ((void *)0);

    if (!x || !y || !result) _Checked {
        _Unchecked { printf("Memory allocation failed.\n"); };
        exit(1);
    }

    while (1) {
        displayMenu();
        scanf("%d", &choice);

        // Exit choice
        if (choice == 5) _Checked {
            _Unchecked { printf("Exiting...\n"); };
            break;
        }

        // Check if valid choice
        if (choice < 1 || choice > 4) _Checked {
            _Unchecked { printf("Invalid choice!\n"); };
            continue;
        }

        // Get input values
        printf("Enter two numbers: ");
        scanf("%lf %lf", x, y);

        // Assign appropriate function to the function pointer
        switch (choice) _Checked {
            case 1:
                operation = add;
            break;
            case 2:
                operation = subtract;
            break;
            case 3:
                operation = multiply;
            break;
            case 4:
                if (*y == 0) {
                    _Unchecked { printf("Error: Division by zero!\n"); };
                    continue;
                }
            operation = divide;
            break;
        }

        // Execute the operation
        *result = executeOperation(operation, x, y);
        printf("Result: %lf\n", *result);
    }

    t_free<double>(x);
    free<double>(y);
    free<double>(result);

    return 0;
}

double add(_TArray_ptr<double> a, _Ptr<double> b) _Checked {
return *a + *b;
}

double subtract(_TArray_ptr<double> a, _Ptr<double> b) _Checked {
return *a - *b;
}

double multiply(_TArray_ptr<double> a, _Ptr<double> b) _Checked {
return *a * *b;
}

double divide(_TArray_ptr<double> a, _Ptr<double> b) _Checked {
return *a / *b;
}

_Tainted void displayMenu() {
    printf("\nCalculator Menu:\n");
    printf("1. Add\n");
    printf("2. Subtract\n");
    printf("3. Multiply\n");
    printf("4. Divide\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

double executeOperation(_Ptr<double (_TArray_ptr<double>, _Ptr<double>)> operation, _TArray_ptr<double> a, _Ptr<double> b) _Checked {
return operation(a, b);
}