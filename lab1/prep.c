// includes here...

typedef struct {
    int rows; // number of rows
    int cols; // number of colum
    double* a; // pointer to rows*cols elements
} Matrix;

// initialize matrix pointed to by m with r rows and c columns
// i.e. allocate sufficient memory and set all elements to 0
void init(Matrix* m, int rows, int cols);

// free memory associated with matrix pointed to by m
void deallocate(Matrix* m);

// set the value of the element at (row, col) to the input value
void set(Matrix* m, int row, int col, double value);

// returns a pointer-to-const to the value of the element at (row, col)
const double* get(const Matrix* m, int row, int col);

// print the value of the element at (row, col) to console with 2 decimal places
void printCell(const Matrix* m, int row, int col);

// print the entire matrix in the following format with 2 decimal places
// i.e. 1.00 2.00 3.00
//      4.00 5.00 6.00
//      7.00 8.00 9.00
void printMatrix(const Matrix* m);

int main(void)
{
    const int rows = 3;
    const int cols = 3;

    Matrix m = { 0 };
    init(&m, rows, cols);

    // Fill the matrix with values
    double value = 1.00;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            set(&m, i, j, value++);
        }
    }

    // Test your Matrix here
    // i.e. printCell() and printMatrix()

    deallocate(&m);
    return 0;
}