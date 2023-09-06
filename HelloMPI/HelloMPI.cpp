#include <stdio.h>
#include "mpi.h"

using namespace std;

int main(int argc, char *argv[]){
	
	int numtasks, rank;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	printf("Hello MPI from process - %d, total number of processes:  %d\n", rank, numtasks);

	MPI_Finalize();

}