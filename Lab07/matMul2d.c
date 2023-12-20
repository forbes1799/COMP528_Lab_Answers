#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAXSIZE 1024			/* change the size of matrices as required*/

void fill_matrix(int *matrix){
    int (*mat)[MAXSIZE] = (int(*)[MAXSIZE]) matrix;
    srand(time(NULL));
	for(int i = 0; i < MAXSIZE; i++){
        for(int j = 0; j < MAXSIZE; j++){
            mat[i][j] = rand() % 10;
        }
    }
}

void print_matrix(int *matrix){
    int (*mat)[MAXSIZE] = (int(*)[MAXSIZE]) matrix;
	for(int i = 0; i < MAXSIZE; i++){
        for(int j = 0; j < MAXSIZE; j++){
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}


int main(int argc, char *argv[])
{
    double tStart, tEnd;

    int my_rank, comm_size, from, to;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int *fullX = (int *)malloc(MAXSIZE * MAXSIZE * sizeof(int));
    int *fullY = (int *)malloc(MAXSIZE * MAXSIZE * sizeof(int));
    int *fullZ = (int *)malloc(MAXSIZE * MAXSIZE * sizeof(int));

    int localRows = MAXSIZE / comm_size;

    int *lclX = (int*)malloc(localRows * MAXSIZE * sizeof(int));
    int *lclZ = (int*)calloc(localRows * MAXSIZE, sizeof(int));

    int i, j, k;
    int root = 0;

    tStart = MPI_Wtime();
	if(MAXSIZE % comm_size != 0){
		exit(-1);
	}

    from = my_rank * localRows;
    to = (my_rank+1) * localRows;

    /*if root rank, fill the matrices X and Y*/

    if(my_rank == root){
        fill_matrix(fullX);
        fill_matrix(fullY);
        print_matrix(fullY);
        print_matrix(fullX);
    }
  
    /*What's the difference here between MPI_Bcast and MPI_Scatter*/
    MPI_Bcast(fullY, MAXSIZE * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    MPI_Scatter(fullX, localRows * MAXSIZE, MPI_INT, lclX, localRows * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    /*parallelise here using OpenMP: fastest time wins!!!! Use clauses and anythign at your disposal. Change the code if you want to.
    Consider NUMA, consider the chunk size of your schedules. Experiment!!!!!!!*/

    int (*localX)[MAXSIZE] = (int(*)[MAXSIZE])lclX;
    int (*localZ)[MAXSIZE] = (int(*)[MAXSIZE])lclZ;
    int (*Y)[MAXSIZE]= (int(*)[MAXSIZE])fullY;
    
    #pragma omp parallel for collapse(3)
    for (i=from; i<to; i++){ 
        for (j=0; j<MAXSIZE; j++) {
            for (k=0; k<MAXSIZE; k++){
                #pragma omp atomic
                localZ[i - from][j] += localX[i - from][k]*Y[k][j];
            }
        }
    }


    MPI_Gather(lclZ, localRows * MAXSIZE, MPI_INT, fullZ, localRows * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    tEnd = MPI_Wtime();

    /*if root print mat Z*/
    if (my_rank == 0){
        print_matrix(fullZ);
        printf("\nProgram took %lf milliseconds\n", (tEnd - tStart) * 1000);
    }

    MPI_Finalize();
    return 0;
}
