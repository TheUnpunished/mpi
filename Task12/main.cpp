#include <iostream>
#include <ctime>
#include <thread>
#include "mpi.h"

#define n 8
#define size 4

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int real_size;
    MPI_Comm_size(MPI_COMM_WORLD, &real_size);
    int *b_temp = (int*) malloc(sizeof(int) * n * n / size);
    if(real_size != size){
        if(rank == 0){
            printf("Это приложение запускается только с 4 процессами\n");
        }
        MPI_Finalize();
        return 0;
    }
    if(rank == 0){
        int a[n][n];
        struct myStruct{
            int a[n][n];
        };
        MPI_Datatype custom;
        int lenghts[2] = {n, n};
        MPI_Datatype types[] = {MPI_INT, MPI_INT};
        MPI_Aint disp[2];

        disp[0] = offsetof(myStruct, a[0]);
        disp[1] = offsetof(myStruct, a[size]);
        MPI_Type_create_struct(2,
                               lenghts,
                               disp,
                               types,
                               &custom);
        MPI_Type_commit(&custom);
        srand(std::time(nullptr));
        printf("Я процесс 0. Отправим на другие процессы следующую матрицу, разделяя строки:\n");
        for(int i = 0; i < n; i ++){
            for(int j = 0; j < n; j ++){
                a[i][j] = std::rand() % 10001;
                printf("%7d", a[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
        MPI_Status status;
        MPI_Sendrecv(&a[0][0], 1, custom, 0, 0,
                     b_temp, n * n / size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        for(int i = 1; i < size; i ++){
            MPI_Send(&a[i % size][0], 1, custom, i, 0, MPI_COMM_WORLD);
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds((rank + 1) * 2));
    printf("Я процесс %d. Получил следующую матрицу:\n", rank);
    if(rank != 0){
        MPI_Recv(b_temp, n * (n / size), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    }
    int b[n / size][n];
    for(int i = 0; i < n / size; i ++){
        for(int j = 0; j < n; j ++){
            b[i][j] = b_temp[i * n + j];
            printf("%7d", b[i][j]);
        }
        printf("\n");
    }
    free(b_temp);
    printf("----------------------------------------------------------------------------------------\n");
    MPI_Finalize();
    return 0;
}
