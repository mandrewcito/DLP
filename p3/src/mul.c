//Author: Vasanth Raja
//Program to multiply two matrices using OpenCL in GPU

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 1024

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void printm(char *name, float *M, int w, int h)
{
	int i, j;

	printf("Matrix %s:\n", name);

	for(i = 0; i < w; i++)
	{
		for(j = 0; j < h; j++)
		{
			printf("%6.0f ", M[i*h + j]);
		}
		printf("\n");
	}
}

double usec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((double)tv.tv_sec) * 1000000.0 + ((double)tv.tv_usec);

}

int main(int argc, char *argv[])
{
	int n = N;
	int nl = 16;
	
	if(argc >= 2)
	{
		n = atoi(argv[1]);
	}
	if(argc >= 3)
	{
		nl = atoi(argv[2]);
	}
	int widthA, heightA, widthB, heightB, widthC, heightC;
	widthA = heightA = widthB = heightB = widthC = heightC = n;


	float * A = (float *)malloc(sizeof(float)*widthA*heightA);
	float * B = (float *)malloc(sizeof(float)*widthB*heightB);
	float * C = (float *)malloc(sizeof(float)*widthC*heightC);
	float * D = (float *)malloc(sizeof(float)*widthC*heightC);

	double tic, toc;
	double tgpu, tcpu;

	int i, j, k;

	for(i = 0;i < widthA; i++)
	{
		for(j=0;j < heightA; j++)
		{
			A[i*heightA+j] = i;
			B[i*heightA+j] = j;
		}
	}
	
	printf("Tamaño de matriz: %dx%d\n", n, n);
	printf("Tamaño de grupo: %dx%d\n", nl, nl);

	//printm("A", A, widthA, heightA);
	//printm("B", B, widthB, heightB);
	

	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobjA = NULL;
	cl_mem memobjB = NULL;
	cl_mem memobjC = NULL;
	cl_mem rowA = NULL;
	cl_mem colC = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	FILE *fp;
	char fileName[] = "matrix_mul.cl";
	char *source_str;
	size_t source_size;
	int row = widthA;
	int col = heightC;
	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose( fp );

	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	memobjA = clCreateBuffer(context, CL_MEM_READ_ONLY, widthA * heightA * sizeof(float), NULL, &ret);
	memobjB = clCreateBuffer(context, CL_MEM_READ_ONLY, widthB * heightB * sizeof(float), NULL, &ret);
	memobjC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, widthC * heightC * sizeof(float), NULL, &ret);
	rowA = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(int), NULL, &ret);
	colC = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(int), NULL, &ret);
	
	toc = usec();
	tic = usec();

	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue,memobjA, CL_TRUE, 0,
			widthA * heightA * sizeof(int), A, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobjB, CL_TRUE, 0,
			widthB * heightB * sizeof(int), B, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, rowA, CL_TRUE, 0, sizeof(int), &row, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, colC, CL_TRUE, 0, sizeof(int), &col, 0, NULL, NULL);
	
	tic = usec() - tic;
	printf("GPU envío:\t%E us\n", tic);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
			(const size_t *)&source_size, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "matrix_mul", &ret);

	/* Set OpenCL Kernel Arguments */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjB);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjC);
	//ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&row);
	ret = clSetKernelArg(kernel, 4, sizeof(int), (void *)&col);
	/* Execute OpenCL Kernel */
	//ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
	size_t globalThreads[2] = {widthA, heightB};
	size_t localThreads[2] = {nl,nl};

	tic = usec();
	
	clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalThreads, localThreads, 0, NULL, NULL);

	tic = usec() - tic;
	printf("GPU comp.:\t%E us\n", tic);
	
	tic = usec();

	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, memobjC, CL_TRUE, 0,
			widthA * heightC * sizeof(float),C, 0, NULL, NULL);
	
	toc = usec() - toc;
	tic = usec() - tic;
	printf("GPU recep.:\t%E us\n", tic);
	printf("GPU total:\t%E us\n", toc);
	printf("GPU total:\t%f us\n", toc);
	tgpu = toc;

	//printm("C", C, widthC, heightC);

	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobjA);
	ret = clReleaseMemObject(memobjB);
	ret = clReleaseMemObject(memobjC);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	

	free(source_str);

	float sum=0.0;
	
	tic = usec();

	for(i = 0;i < widthA; i++)
	{
		for(j = 0; j < heightC; j++)
		{
			sum = 0;
			for(k = 0; k < widthB; k++)
			{
				sum += A[i*col+k] * B[k*row+j];
			}
			D[i*heightC+j] = sum;
		}

	}
	
	tic = usec() - tic;
	printf("CPU comp.:\t%E us\n", tic);
	printf("CPU comp.:\t%f us\n", tic);
	tcpu = tic;
	
	printf("Aceleración:\t%f\n", tcpu/tgpu);
	

	int fallos = 0;
	for(i = 0;i < widthC; i++)
	{
		for(j = 0; j < heightC; j++)
		{
			if(D[i*heightC+j] != C[i*heightC+j])
			{
				fallos++;
			}
		}
	}

	printf("Fallos comp.:\t%d\n", fallos);

	return 0;
}
