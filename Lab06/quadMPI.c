#include <mpi.h>
#include <stdio.h>
#include <math.h>

void integrationPTP(int, int);
void integrationCC(int, int);
double indieRankWork(int, int);
double func(double);

int main(void){
    int commSz;
    int myRank;

    const int REPEAT = 25;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commSz);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    double tStart;
    double tEnd;
	
    int count = 0;
    for(count; count < REPEAT; count++){
		
	
    	tStart = MPI_Wtime();
        integrationPTP(commSz, myRank);
		tEnd = MPI_Wtime();

		if(myRank == 0){
			printf("COUNT = %d\n", count);
			printf("Time taken for PTP = %f milliseconds\n", 1000.0*(tEnd - tStart));
		}
		
		tStart = MPI_Wtime();
		integrationCC(commSz, myRank);
		tEnd = MPI_Wtime();

		if(myRank == 0){
			printf("Time taken for CC = %f milliseconds\n", 1000.0*(tEnd - tStart));
			printf("----------------\n");
		}
		
    }	

    MPI_Finalize();

}

void integrationPTP(int commSz, int myRank){
    
	
	double mySum = indieRankWork(commSz, myRank);

	if(myRank != 0){
		MPI_Send(&mySum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	else{
		double totalSum = mySum;
		double recvSum;
		int i;
		for(i = 1; i < commSz; i++){
			MPI_Recv(&recvSum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			totalSum += recvSum;
		}
		printf("Final area is %f\n", totalSum);
	}

	MPI_Barrier(MPI_COMM_WORLD);

}

void integrationCC(int commSz, int myRank){
	double mySum = indieRankWork(commSz, myRank);

	double totalSum;

	MPI_Reduce(&mySum, &totalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(myRank == 0){
		printf("Final area is %f\n", totalSum);
	}
}

double indieRankWork(int commSz, int myRank){
	const double a=0.0, b=3.1415265359;
	const int  quads = 50000;
	double const width = (b-a) / (double) quads;

	int quadsPerRank = quads / commSz;
	
	if(myRank == commSz - 1){
		quadsPerRank = quads - (quadsPerRank*myRank);
	}
	
	int startIter = myRank * quadsPerRank;
	int endIter = startIter + quadsPerRank - 1;
	
	double x, y;
    
	double sum;
	int i;
    for(i = startIter; i <= endIter; i++){
		x = a + i * width;
		y = x + width;
		sum += 0.5*width*(func(x) + func(y));
    }
	return sum;
}

