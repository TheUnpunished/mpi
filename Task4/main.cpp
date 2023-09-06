#include <iostream>
#include "mpi.h"
#include "ctime"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    // флажочек для разных вариантов. true - сложение, false - умножение
    bool flag = true;
    int n;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0){
        std:srand(std::time(nullptr));
        n = std::rand() % 15 + 5;
        // Делаем так, чтобы размер массива делился поровну
        // Вычитаем остаток деления от size - 1
        n = n - (n % (size - 1));
        //  определяем x и y и выводим данные на экран
        int x[n];
        int y[n];
        printf("Я процесс 0. Попытаемся выделить память на остальных процессах и отправить массивы: \n");
        for (int i = 0; i < n; i++){
            x[i] = std::rand() % 10000;
            y[i] = std::rand() % 10000;
        }
        for (int i = 0; i < n; i ++)
            printf("%d ", x[i]);
        printf("\n");
        for (int i = 0; i < n; i ++)
            printf("%d ", y[i]);
        // Отправляем данные остальным процессам
        for (int i = 1; i < size; i++){
            int x_temp[n/(size - 1)];
            int y_temp[n/(size - 1)];
            for (int j = 0; j < (n/(size - 1)); j++){
                x_temp[j] = x[j + n/(size - 1)*(i - 1)];
                y_temp[j] = y[j + n/(size - 1)*(i - 1)];
            }
            MPI_Send(x_temp, n/(size - 1), MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(y_temp, n/(size - 1), MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        printf("\n----------------------------------------------------------------------------------------\n");
        // 0 процесс спит 10 секунд. Больше сделано для синхронизации вывода
        std::this_thread::sleep_for(std::chrono::seconds(10));
        int messages[size - 1][n/(size - 1)];
        int z[n];
        // получаем данные от другиъ процессов и выводим их на экран
        for (int i = 1; i < size; i++) {
            MPI_Recv(messages[i], n / (size - 1), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            for (int j = 0; j < n/(size - 1); j++){
                z[j + n/(size - 1)*(i - 1)] = messages[i][j];
            }
        }
        printf("Процесс 0 получил от остальных переделанный массив: \n");
        for(int i = 0; i < n; i++)
            printf("%d ", z[i]);
    }
    else{
        // остальные спят ранк * 2: 2, 4, 6, 8 итд
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        //  Динамически получаем данные о полученной информации
        // выделяем память и получаем непосредственно их
        MPI_Status status_x;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status_x);
        MPI_Get_count(&status_x, MPI_INT, &n);
        int* x = (int*)malloc(sizeof(int) * n);
        int* y = (int*)malloc(sizeof(int) * n);
        MPI_Recv(x, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Recv(y, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        // Вывод и обработка по заданию
        printf("Я - процесс %d. Получил 2 массива %d чисел:\n", rank, n);
        for (int i = 0; i < n; i++)
            printf("%d ", x[i]);
        printf("\n");
        for (int i = 0; i < n; i++)
            printf("%d ", y[i]);
        printf("\n----------------------------------------------------------------------------------------\n");
        int z[n];
        if(flag)
            for (int i = 0; i < n; i++){
                z[i] = x[i] + y[i];
        }
        else
            for (int i = 0; i < n; i++){
                z[i] = x[i] * y[i];
        }
        // Обратно отправляем
        for(int i = 1; i < size; i++){
            if(i == rank){
                MPI_Send(z, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
