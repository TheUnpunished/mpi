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
    double * x_temp;
    double * x;
    if(rank == 0){
        std::srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
        n = n - (n % size);
        x = (double *) malloc(sizeof(double) * n);
        x_temp = (double *) malloc(sizeof(double) * (n / size));
        printf("Я процесс 0. Попытаемся выделить память на остальных процессах и распределить матрицу построчно: \n");
        for (int i = 0; i < n; i++) {
            x[i] = ((double) (std::rand() % 1000001) / 100) - 5000;
            printf("%.2f ", x[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
        for(int i = 1; i < size; i ++)
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        x_temp = (double*) malloc(sizeof(double) * (n / size));
    }
    MPI_Scatter(x, n / size, MPI_DOUBLE, x_temp, n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    std::this_thread::sleep_for(std::chrono::seconds(rank * 2 + 1));
    printf("Я - процесс %d. Получил массив %d чисел: \n", rank, n / size);
    for(int i = 0; i < n / size; i ++){
        printf("%.2f ", x_temp[i]);
    }
    if (rank != 0){}
    printf("\n----------------------------------------------------------------------------------------\n");
    struct {
        double val = std::numeric_limits<double>::max();
        int   i;
    } min_local;
    for (int i = 0; i < n / size; i ++)
        if(x_temp[i] < min_local.val){
            min_local.val = x_temp[i];
            min_local.i = n / size * rank + i;
        }
    struct {
        double val;
        int   i;
    } value;
    MPI_Reduce(&min_local, &value, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
    if(rank == 0){
        printf("Минимальное число: %.2f\n"
               "Глобальный индекс (из общего массива): %d\n", value.val, value.i);
    }
    MPI_Finalize();
    return 0;
}