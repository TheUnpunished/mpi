#include <iostream>
#include "mpi.h"
#include "ctime"
#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
    // Инициализируем MPI
    MPI_Init(&argc, &argv);
    // Узнаём "ранк" - номер процесса
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Объявляем n-число чисел для всех процессов
    int n;
    // Определяем алгоритм для 1 процесса
    if(rank == 1){
        std:srand(std::time(nullptr));
        // Рандомно определяем число чисел в массиве
        n = std::rand() % 5 + 5;
        int array[n];
        printf("Попытаемся выделить память на остальных процессах и отправить массив: \n");
        // Определяем числа в массиве и выводим их на экран
        for(int i = 0; i < n; i++){
            array[i] = (int) std::rand();
            printf("%d ", array[i]);
        }
        // Узнаём количество процессов и отправляем массив на все, кроме 1го
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        for(int i = 0; i < size; i++){
            if(i != 1){
                MPI_Send(array, n, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    else{
        // Остальные процессы спят секунду
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Объявляем и узнаём "статус отправки", чтобы узнать, сколько информации нам пришло
        MPI_Status status;
        MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
        // Получаем информацию о количестве данных
        MPI_Get_count(&status, MPI_INT, &n);
        // Выделяем память под массив и получаем его
        int* buffer = (int*)malloc(sizeof(int) * n);
        MPI_Recv(buffer, n, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        // Выводим массив на экран
        printf("Я - процесс %d. Получил %d чисел:\n", rank, n);
        for (int i = 0; i < n; i++){
            printf("%d ", buffer[i]);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
    }
    // Финализируем MPI
    MPI_Finalize();
    return 0;
}
