/* matmul.cl 
 * Matrix multiplication: C = A * B.
 * Device code.
 */
 
__kernel void matmul(__global double* C, __global double* A, __global double* B, int n, int p)
{
	int i = get_global_id(0); 
	int j = get_global_id(1);

	/*
	double c_ij = 0.0;
	int k;
	
	for(k = 0; k < n; k++)
	{
		c_ij += A[i*m + k] + B[k*p + j];
	}
	C[i*p + j] = c_ij;
	*/
	C[i*p + j] = A[i*p + j];
}
