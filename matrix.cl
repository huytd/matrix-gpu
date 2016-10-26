#define MATRIX_SIZE 1000

kernel void hello(global ulong *A, global ulong *B, global ulong *C) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    ulong sum = 0;
    for (int k = 0; k < MATRIX_SIZE; k++) {
        sum += A[row * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + col];
    }
    C[row * MATRIX_SIZE + col] = sum;
}
