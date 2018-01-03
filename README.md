# Matrix Multiplication on GPU

There are 2 programs: `cpu` and `gpu`. Both of them does the same thing is multiply two `1000 x 1000` matrices.

The algorithm to multiply is simple:

```c
for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      ulong sum = 0;
      for (int k = 0; k < MATRIX_SIZE; k++) {
        sum += A[row * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + col];
      }
      C[row * MATRIX_SIZE + col] = sum;
    }
  }
```

The GPU program uses OpenCL.

## How to run

On macOS, compile with Make:

```
make
```

Them run the CPU program with:

```
./cpu
```

Run the GPU program with:

```
./gpu
```

## Compare the result

To compare the result of the two methods, do the following steps:

**Step 1:** Run CPU code and save the output to `output_cpu` file:

```
./cpu > output_cpu
```

**Step 2:** Run GPU code and save the output to `output_gpu` file:

```
./gpu > output_gpu
```

**Step 3:** Use `diff` to compare the two results to make sure outputs are the same:

```
diff output_cpu output_gpu
```

The only different is the running time. For example:

```
< Runtime: 4.849347ms
---
> Runtime: 0.001779ms
```
