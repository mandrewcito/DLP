// Multiply two matrices A * B = C
 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif
 
#define MAX_SOURCE_SIZE (0x100000)
#define N 100
 
// Allocates a matrix with random float entries.
void random_init(double* data, size_t size)
{
	int i;
	for (i = 0; i < size; i++)
		data[i] = drand48();
}
 
/////////////////////////////////////////////////////////
// Program main
/////////////////////////////////////////////////////////
 
int main(int argc, char** argv)
{
	// 1. allocate host memory for matrices A and B
	size_t size_A, size_B, size_C;
	size_t mem_size_A, mem_size_B, mem_size_C;
	double *A, *B, *C;
	int i;
	
	size_A = size_B = size_C = N * N;
	mem_size_A = mem_size_B = mem_size_C = N * N * sizeof(double);

	A = malloc(mem_size_A);
	B = malloc(mem_size_B);
	C = malloc(mem_size_C);

	// 2. initialize host memory
	random_init(A, size_A);
	random_init(B, size_B);

	for(i=0; i<10; i++)
	{
		printf("%2.2f ", A[i]);
	}
	printf("\n");
	for(i=0; i<10; i++)
	{
		printf("%2.2f ", B[i]);
	}
	printf("\n");


	// 5. Initialize OpenCL
	// OpenCL specific variables
	cl_context clGPUContext;
	cl_device_id device_id;
	cl_command_queue clCommandQue;
	cl_program clProgram;
	cl_kernel clKernel;

	size_t dataBytes;
	size_t kernelLength;
	cl_int errcode;

	// OpenCL device memory for matrices
	cl_mem d_A;
	cl_mem d_B;
	cl_mem d_C;

	/*****************************************/
	/* Initialize OpenCL */
	/*****************************************/
	/*
	clGPUContext = clCreateContextFromType(0, 
		CL_DEVICE_TYPE_GPU, 
		NULL, NULL, &errcode);
	
	if(clGPUContext == 0)
	{
		perror("clCreateContextFromType");
		return 1;
	}

	// get the list of GPU devices associated 
	// with context
	errcode = clGetContextInfo(clGPUContext, 
		CL_CONTEXT_DEVICES, 0, NULL, 
		&dataBytes);
	
	cl_device_id *clDevices = (cl_device_id *)
		malloc(dataBytes);
	
	errcode |= clGetContextInfo(clGPUContext, 
		CL_CONTEXT_DEVICES, dataBytes, 
		clDevices, NULL);
	*/
	clGPUContext = clCreateContext(NULL, 1, &device_id, NULL, NULL, &errcode);

	//Create a command-queue
	clCommandQue = clCreateCommandQueue(clGPUContext, 
		device_id, 0, &errcode);

	// Setup device memory
	d_C = clCreateBuffer(clGPUContext, 
		CL_MEM_READ_WRITE, 
		mem_size_C, NULL, &errcode);

	d_A = clCreateBuffer(clGPUContext, 
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
		mem_size_A, &A, &errcode);
	
	d_B = clCreateBuffer(clGPUContext, 
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
		mem_size_B, &B, &errcode);


	// 6. Load and build OpenCL kernel

	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("matmul.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*) malloc(MAX_SOURCE_SIZE);
	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose( fp );



	clProgram = clCreateProgramWithSource(clGPUContext, 
			1, (const char **)&source_str, 
			&source_size, &errcode);

	errcode = clBuildProgram(clProgram, 0, 
			NULL, NULL, NULL, NULL);

	clKernel = clCreateKernel(clProgram, 
			"matmul", &errcode);


	// 7. Launch OpenCL kernel
	size_t localWorkSize[2], globalWorkSize[2];

	int wA = N;
	int wC = N;
	errcode = clSetKernelArg(clKernel, 0, 
			sizeof(cl_mem), (void *)&d_C);
	errcode |= clSetKernelArg(clKernel, 1, 
			sizeof(cl_mem), (void *)&d_A);
	errcode |= clSetKernelArg(clKernel, 2, 
			sizeof(cl_mem), (void *)&d_B);
	errcode |= clSetKernelArg(clKernel, 3, 
			sizeof(int), (void *)&wA);
	errcode |= clSetKernelArg(clKernel, 4, 
			sizeof(int), (void *)&wC);

	globalWorkSize[0] = N;
	globalWorkSize[1] = N;

	errcode = clEnqueueNDRangeKernel(clCommandQue, 
			clKernel, 2, NULL, globalWorkSize, 
			NULL, 0, NULL, NULL);

	// 8. Retrieve result from device
	errcode = clEnqueueReadBuffer(clCommandQue, 
			d_C, CL_TRUE, 0, mem_size_C, 
			C, 0, NULL, NULL);

	// 9. print out the results
	
	printf("\n\nMatrix C (Results)\n");
	for(i = 0; i < 10; i++)
	{
		printf("%2.2f ", C[i]);
	}
	printf("\n");

	// 10. clean up memory
	free(A);
	free(B);
	free(C);

	clReleaseMemObject(d_A);
	clReleaseMemObject(d_C);
	clReleaseMemObject(d_B);

	//free(clDevices);
	//free(clMatrixMul);
	clReleaseContext(clGPUContext);
	clReleaseKernel(clKernel);
	clReleaseProgram(clProgram);
	clReleaseCommandQueue(clCommandQue);

}
