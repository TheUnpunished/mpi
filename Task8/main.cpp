#include <iostream>
#include <ctime>
#include <thread>
#include "mpi.h"

int main(int argc, char* argv[]) {
    int n;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int* x;
    int* y;
    int * x_temp;
    int * y_temp;
    int local_sum = 0;
    if(rank == 0){
        std:srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
        n = n - (n % size);
        x = (int*) malloc(sizeof(int) * n);
        y = (int*) malloc(sizeof(int) * n);
        x_temp = (int*) malloc(sizeof(int) * (n / size));
        y_temp = (int*) malloc(sizeof(int) * (n / size));
        printf("Я процесс 0. Попытаемся выделить память на остальных процессах и распределить 2 вектора: \n");
        printf("Вектор x: ");
        for (int i = 0; i < n; i++){
            x[i] = ((std::rand() % 10001)) - 5000;
            printf("%d ", x[i]);
        }
        printf("\nВектор y: ");
        for (int i = 0; i < n; i++){
            y[i] = ((std::rand() % 10001)) - 5000;
            printf("%d ", y[i]);
        }
        printf("\nВычисляем их скалярное произведение...");
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("Я - процесс %d. Получил 2 массива %d чисел.", rank, n / size);
        printf("\n----------------------------------------------------------------------------------------\n");
        for(int i = 1; i < size; i ++)
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else{
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        x_temp = (int*) malloc(sizeof(int) * (n / size));
        y_temp = (int*) malloc(sizeof(int) * (n / size));
        printf("Я - процесс %d. Получил 2 массива %d чисел.", rank, n / size);
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    MPI_Scatter(x, n / size, MPI_INT, x_temp, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, n / size, MPI_INT, y_temp, n / size, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < n / size; i ++){
        local_sum += x_temp[i] * y_temp[i];
    }
    int sum;
    MPI_Reduce(&local_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0){
        printf("Скалярное произведение векторов x и y: %d\n", sum);
    }
    MPI_Finalize();
    return 0;
}
