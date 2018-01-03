#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define MATRIX_SIZE 1000

cl_ulong A[MATRIX_SIZE * MATRIX_SIZE];
cl_ulong B[MATRIX_SIZE * MATRIX_SIZE];
cl_ulong C[MATRIX_SIZE * MATRIX_SIZE];

int main() {
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;

  cl_mem memobj_A = NULL;
  cl_mem memobj_B = NULL;
  cl_mem memobj_C = NULL;

  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;

  for (cl_ulong i = 0; i < MATRIX_SIZE; i++) {
    for (cl_ulong j = 0; j < MATRIX_SIZE; j++) {
      A[i * MATRIX_SIZE + j] = 1;
      B[i * MATRIX_SIZE + j] = 2;
      C[i * MATRIX_SIZE + j] = 0;
    }
  }

  FILE *fp;
  char fileName[] = "./matrix.cl";
  char *source_str;
  size_t source_size;
  fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel\n");
    exit(1);
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
  command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

  memobj_A = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), NULL, &ret);
  memobj_B = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), NULL, &ret);
  memobj_C = clCreateBuffer(context, CL_MEM_READ_WRITE, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), NULL, &ret);

  program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  kernel = clCreateKernel(program, "hello", &ret);

  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memobj_A);
  ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&memobj_B);
  ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&memobj_C);
  
  ret = clEnqueueWriteBuffer(command_queue, memobj_A, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), A, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, memobj_B, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), B, 0, NULL, NULL);

  clock_t begin = clock();

  size_t globalWorkSize[2] = {MATRIX_SIZE, MATRIX_SIZE};
  size_t localWorkSize[2] = {1, 1};

  ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
  ret = clEnqueueReadBuffer(command_queue, memobj_C, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(cl_mem), C, 0, NULL, NULL);

  clock_t end = clock();
  double runtime = (double)(end - begin) / CLOCKS_PER_SEC;

  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(memobj_A);
  ret = clReleaseMemObject(memobj_B);
  ret = clReleaseMemObject(memobj_C);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      printf("%llu ", C[i * MATRIX_SIZE + j]);
    }
    printf("\n");
  }

  printf("Runtime: %lfms\n", runtime);
  
  free(source_str);

  return 0;
}
