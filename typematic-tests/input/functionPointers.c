#include <stdio.h>
#include <stdlib.h>
#include <stdlib_tainted.h>

// Function prototypes
double add(double *a, double *b);
double subtract(double *a, double *b);
double multiply(double *a, double *b);
double divide(double *a, double *b);
void displayMenu();
double executeOperation(double (*operation)(double *, double *), double *a, double *b);

int main() {
    int choice;
    _TPtr<double> x = malloc(sizeof(double));
    double *y = malloc(sizeof(double));
    double *result = malloc(sizeof(double));
    double (*operation)(double *, double *);

    if (!x || !y || !result) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    while (1) {
        displayMenu();
        scanf("%d", &choice);

        // Exit choice
        if (choice == 5) {
            printf("Exiting...\n");
            break;
        }

        // Check if valid choice
        if (choice < 1 || choice > 4) {
            printf("Invalid choice!\n");
            continue;
        }

        // Get input values
        printf("Enter two numbers: ");
        scanf("%lf %lf", x, y);

        // Assign appropriate function to the function pointer
        switch (choice) {
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
                    printf("Error: Division by zero!\n");
                    continue;
                }
                operation = divide;
                break;
        }

        // Execute the operation
        *result = executeOperation(operation, x, y);
        printf("Result: %lf\n", *result);
    }

    free(x);
    free(y);
    free(result);

    return 0;
}

double add(double *a, double *b) {
    return *a + *b;
}

double subtract(double *a, double *b) {
    return *a - *b;
}

double multiply(double *a, double *b) {
    return *a * *b;
}

double divide(double *a, double *b) {
    return *a / *b;
}

void displayMenu() {
    printf("\nCalculator Menu:\n");
    printf("1. Add\n");
    printf("2. Subtract\n");
    printf("3. Multiply\n");
    printf("4. Divide\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

double executeOperation(double (*operation)(double *, double *), double *a, double *b) {
    return operation(a, b);
}
