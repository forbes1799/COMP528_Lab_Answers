#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAXSIZE 8			/* change the size of matrices as required*/

int X[MAXSIZE][MAXSIZE], Y[MAXSIZE][MAXSIZE], Z[MAXSIZE][MAXSIZE];

void fill_matrix(int mat[MAXSIZE][MAXSIZE]){
    srand(time(NULL));
	for(int i = 0; i < MAXSIZE; i++){
        for(int j = 0; j < MAXSIZE; j++){
            mat[i][j] = rand() % 10;
        }
    }
}

void print_matrix(int mat[MAXSIZE][MAXSIZE])
{
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
    
    int part_size = MAXSIZE*MAXSIZE/comm_size;

    int localX[part_size][MAXSIZE], localY[part_size][MAXSIZE], localZ[part_size][MAXSIZE];

    int i, j, k;
    int root = 0;

    tStart = MPI_Wtime();
    
	if(MAXSIZE % comm_size != 0){
		exit(-1);
	}

    from = my_rank * part_size;
    to = (my_rank+1) * part_size;

    /*if root rank, fill the matrices X and Y*/

    if(my_rank == root){
        printf("Here?");
        fill_matrix(X);
        fill_matrix(Y);
        print_matrix(Y);
        print_matrix(X);
    }
  
    /*What's the difference here between MPI_Bcast and MPI_Scatter*/
    MPI_Bcast(Y, MAXSIZE * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    

    MPI_Scatter (X, part_size, MPI_INT, localX, part_size * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);
    

    /*parallelise here using OpenMP: fastest time wins!!!! Use clauses and anythign at your disposal. Change the code if you want to.
    Consider NUMA, consider the chunk size of your schedules. Experiment!!!!!!!*/
    for (i=from; i<to; i++){ 
        for (j=0; j<MAXSIZE; j++) {
            localZ[i - from][j]=0;
            for (k=0; k<MAXSIZE; k++){
                localZ[i - from][j] += localX[i - from][k]*Y[k][j];
            }
        }
    }


    MPI_Gather(localZ, part_size, MPI_INT, Z, part_size * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    /*if root print mat Z*/
    if (my_rank == 0){
        print_matrix(Z);
        printf("\nProgram took %lf milliseconds\n", (tEnd - tStart) * 1000);
    }

    tEnd = MPI_Wtime();

    

    MPI_Finalize();
    return 0;
}
