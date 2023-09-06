#include <iostream>
#include <ctime>
#include <thread>
#include "mpi.h"
#include "chrono"

int main(int argc, char* argv[]) {
    int n = 8;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0){
        MPI_Datatype vector;
        MPI_Type_vector(n / 2, n, n, MPI_INT, &vector);
        MPI_Type_commit(&vector);
        int a[n][n];
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
        for(int i = 1; i < size; i ++){
            MPI_Send(&a[i % 2][0], 1, vector, i, 0, MPI_COMM_WORLD);
        }
    }
    else{
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        printf("Я процесс %d. Получил следующую матрицу:\n", rank);
        int *b_temp = (int*) malloc(sizeof(int) * n * n / 2);
        MPI_Recv(b_temp, n * n / 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        int b[n / 2][n];
        for(int i = 0; i < n / 2; i ++){
            for(int j = 0; j < n; j ++){
                b[i][j] = b_temp[i * n + j];
                printf("%7d", b[i][j]);
            }
            printf("\n");
        }
        free(b_temp);
        printf("----------------------------------------------------------------------------------------\n");
    }
    MPI_Finalize();
}
