#include "mpi.h"
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>


int main(int argc, char *argv[]){
	
	MPI_Init(&argc, &argv);
	int world_rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int array[10];
	if (world_rank == 0) {
		std::srand(std::time(nullptr));
		printf("I'm process #%d, I'm sending this array to other processes:\n", world_rank);
		for (int i = 0; i < 10; i++) {
			array[i] = std::rand();
			printf("%d ", array[i]);
		}
		for (int j = 1; j < world_size; j++) {
			MPI_Send(array, 10, MPI_INT, j, 0, MPI_COMM_WORLD);
		}
		printf("\n----------------------------------------------------------------------------------------\n");

	}
	else {
		printf("I'm process #%d, I'm receiving an array from process #0.\n", world_rank);
		
		MPI_Recv(array, 10, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		printf("Array I've received: \n");
		for (int i = 0; i < 10; i++) {
			printf("%d ", array[i]);
		}
		printf("\n----------------------------------------------------------------------------------------\n");
	}

	MPI_Finalize();

}