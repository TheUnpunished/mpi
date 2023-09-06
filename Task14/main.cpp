#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include "mpi.h"

#define N 6
#define M 8
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int real_size;
    MPI_Comm_size(MPI_COMM_WORLD, &real_size);
    MPI_Aint disp[2];
    MPI_Datatype datatypes[2] = {MPI_DOUBLE, MPI_INT};
    int lenghts[2] = {N, M};
    struct myStruct{
        double double_Array[N];
        int int_Array[M];
    };
    myStruct my;
    disp[0] = offsetof(myStruct, double_Array[0]);
    disp[1] = offsetof(myStruct, int_Array[0]);
    MPI_Datatype custom;
    MPI_Type_create_struct(2, lenghts, disp, datatypes, &custom);
    MPI_Type_commit(&custom);
    if(rank == 0){
        printf("Я процесс 0. Отправим на другие процессы следующие векторы:\n");
        srand(std::time(nullptr));
        for(int i = 0; i < N; i ++){
            my.double_Array[i] = ((double) (std::rand() % 1000001)) / 100.0;
            printf("%7.2f ", my.double_Array[i]);
        }
        printf("\n");
        for(int i = 0; i < M; i ++){
            my.int_Array[i] = std::rand() % 10001;
            printf("%5d ", my.int_Array[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    MPI_Bcast(&my, 1, custom, 0, MPI_COMM_WORLD);
    if(rank != 0){
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        printf("Я - процесс %d, получил следующие векторы: \n", rank);
        int int_Array[M];
        double double_Array[N];
        for(int i = 0; i < N; i ++){
            double_Array[i] = my.double_Array[i];
            printf("%7.2f ", double_Array[i]);
        }
        printf("\n");
        for(int i = 0; i < M; i ++){
            int_Array[i] = my.int_Array[i];
            printf("%5d ", int_Array[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    MPI_Finalize();
    return 0;
}
