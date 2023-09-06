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
    int * x;
    if (rank == 0){
        srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
        n = n - (n % size);
        x = new int[n];
        printf("Я - процесс 0. Попытаемся отправить всем остальным процессам данный вектор: \n");
        for(int i = 0; i < n; i ++){
            x[i] = std::rand() % 10001;
            printf("%6d", x[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank != 0)
        x = (int *) malloc(sizeof(int) * n);
    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    std::this_thread::sleep_for(std::chrono::seconds(rank * 2 + 1));
    if(rank != 0){
        printf("Я - процесс %d, получил следующий вектор: \n", rank);
        for (int i = 0; i < n; i ++)
            printf("%6d", x[i]);
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    std::this_thread::sleep_for(std::chrono::seconds(size * 2 + 1 - (rank * 2 + 1)));
    int A_temp[n / size][n];
    if(rank == 0){
        printf("Я - процесс 0. Попытаемся отправить следующую матрицу остальным процессам,"
               "равномерно распределив её по строкам: \n");
        int A[n][n];
        for(int i = 0; i < n; i ++){
            for(int j = 0; j < n; j ++){
                A[i][j] = std::rand() % 10001;
                printf("%6d", A[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
        MPI_Scatter(A, n / size * n, MPI_INT, A_temp, n / size * n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Scatter(NULL, n / size * n, MPI_INT, A_temp, n / size * n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    std::this_thread::sleep_for(std::chrono::seconds(rank * 2 + 1));
    printf("Я - процесс %d, получил следующую матрицу: \n", rank);
    for (int i = 0; i < n / size; i ++){
        for(int j = 0; j < n; j ++){
            printf("%6d", A_temp[i][j]);
        }
        printf("\n");
    }
    printf("----------------------------------------------------------------------------------------\n");
    long local_result[n / size];
    for(int i = 0; i < n / size; i ++){
        local_result[i] = 0;
        for(int j = 0; j < n; j ++){
            local_result[i] += A_temp[i][j] * x[j];
        }
    }
    if(rank == 0){
        long global_result[n];
        MPI_Gather(local_result, n / size, MPI_LONG, global_result, n / size, MPI_LONG, 0, MPI_COMM_WORLD);
        printf("Результат умножения глобальной матрицы на вектор: \n");
        for(int i = 0; i < n; i ++){
            printf("%ld ", global_result[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    else{
        MPI_Gather(local_result, n / size, MPI_LONG, NULL, n / size, MPI_LONG, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
