#include<stdio.h>
#include<stdlib.h>

__global__ void vectorAdd(float *z, float *x, float *y, float A, int num){
	int i = threadIdx.x + blockIdx.x*blockDim.x;
	while(i < num){
		z[i] = A * x[i] + y[i];
		i += blockDim.x * gridDim.x;
	}
}

int main(void){
	//declaration of host variables
	const int num = 1000000;
	float *z, *x, *y;

	float *z_dev, *x_dev, *y_dev;

	float A = 34;
	
	//initialising host variables
	z = (float*) malloc(num * sizeof(float));
	x = (float*) malloc(num * sizeof(float));
	y = (float*) malloc(num * sizeof(float));

	cudaMalloc(&x_dev, num*sizeof(float));
	cudaMalloc(&y_dev, num*sizeof(float));
	cudaMalloc(&z_dev, num*sizeof(float));


	for(int i = 0; i < num; i++){
		x[i] = i;
		y[i] = 7 * i;
	}

	cudaMemcpy(x_dev, x, num*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(y_dev, y, num*sizeof(float), cudaMemcpyHostToDevice);

	vectorAdd<<<8, 128>>>(z_dev, x_dev, y_dev, A, num);

	cudaMemcpy(z, z_dev, num * sizeof(float), cudaMemcpyDeviceToHost);
	
	//postprocessing: output to terminal
	for(int j = 0; j < num; j++){
		printf("%f ", z[j]);
	}
	
	return 0;
}
