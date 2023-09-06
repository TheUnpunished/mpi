#include <iostream>
#include <ctime>
#include <thread>
#include "mpi.h"
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int real_size;
    MPI_Comm_size(MPI_COMM_WORLD, &real_size);
    int n;
    if(rank == 0){
        srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int a[n][n];
    MPI_Datatype new_type;
    int lengths[n];
    for(int i = 0; i < n; i ++){
        lengths[i] = i + 1;
    }
    int displacements[n];
    for(int i = 0; i < n; i ++){
        displacements[i] = i * n;
    }
    MPI_Type_indexed(n, lengths, displacements, MPI_INT, &new_type);
    MPI_Type_commit(&new_type);
    if(rank == 0){
        printf("Я - процесс 0. Попытаемся отправить всем остальным процессам данную матрицу: \n");
        for(int i = 0; i < n; i ++){
            for(int j = 0; j < n; j ++){
                if(j <= i){
                    a[i][j] = std::rand() % 10001;
                }
                else{
                    a[i][j] = 0;
                }
                printf("%7d", a[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
    }
    MPI_Bcast(a, 1, new_type, 0, MPI_COMM_WORLD);
    if(rank != 0){
        std::this_thread::sleep_for(std::chrono::seconds((rank) * 2));
        printf("Я процесс %d. Получил следующую матрицу:\n", rank);
        for(int i = 0; i < n; i ++){
            for(int j = 0; j < n; j ++){
                if(i < j){
                    a[i][j] = 0;
                }
                printf("%7d", a[i][j]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------\n");
    }
    MPI_Finalize();
    return 0;
}
