#include <iostream>
#include <thread>
#include "mpi.h"
#include "ctime"

int main(int argc, char* argv[]) {
    int n;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int * x_temp;
    int local_sum = 0;
    int* x;
    if(rank == 0){
        std:srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
        n = n - (n % size);
        x = (int*) malloc(sizeof(int) * n);
        x_temp = (int*) malloc(sizeof(int) * (n / size));
        printf("Я процесс 0. Попытаемся выделить память на остальных процессах и распределить массив: \n");
        for (int i = 0; i < n; i++){
            x[i] = ((std::rand() % 10001)) - 5000;
            printf("%d ", x[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("Я - процесс %d. Получил массив %d чисел.", rank, n / size);
        printf("\n----------------------------------------------------------------------------------------\n");
        for(int i = 1; i < size; i ++)
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

    }
    else{
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        x_temp = (int*) malloc(sizeof(int) * (n / size));
        printf("Я - процесс %d. Получил массив %d чисел.", rank, n / size);
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    MPI_Scatter(x, n / size, MPI_INT, x_temp, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < n / size; i ++){
        local_sum += abs(x_temp[i]);
    }
    int sum;
    MPI_Reduce(&local_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0){
        printf("Норма вектора x: %d\n", sum);
    }
    MPI_Finalize();
    return 0;
}
