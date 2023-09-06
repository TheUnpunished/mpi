#include <iostream>
#include "mpi.h"
#include "ctime"
#include "chrono"
#include "thread"
#include "math.h"

int main(int argc, char* argv[]) {
    int n;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0){
        std:srand(std::time(nullptr));
        //  определяем n
        n = std::rand() % 4 + 3;
        // делаем так, чтобы делилось: умножаем количество столбцов на (количество потоков -1)
        int a[n][n * (size - 1)];
        printf("Я процесс 0. Попытаемся выделить память на остальных процессах и отправить массивы: \n");
        // Заполняем и выводим матрицу
        for (int i = 0; i < n; i++){
            for(int j = 0; j < n * n; j++){
                printf("%5d", a[i][j] = std::rand() % 9999 + 1);
            }
            printf("\n");
        }
        // Для каждого отдельного потока отправляем матрицу n * n
        for (int i = 1; i < size; i++){
            int a_temp[n][n];
            for(int j = 0; j < n; j++){
                for(int k = 0; k < n; k++){
                    a_temp[j][k] = a[j][k + (i - 1) * (size -1 )];
                }
            }
            MPI_Send(a_temp, n * n, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        printf("----------------------------------------------------------------------------------------\n");
        // Ждём 10 секунд (псевдосинхронизация, для вывода)
        std::this_thread::sleep_for(std::chrono::seconds(10));
        int messages[size - 1][n];
        // Получаем диагонали обратно
        for(int i = 1; i < size; i++){
            MPI_Recv(messages[i-1], n, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        printf("Получил от остальных переделанные массивы: \n");
        for (int i = 0; i < size - 1; i++){
            for(int j = 0; j < n; j++){
                printf("%5d", messages[i][j]);
            }
            printf("\n");
        }
    }
    else{
         // Ждём 2 секунды * номер процесса: 2, 4, 6 и.т.д.
        std::this_thread::sleep_for(std::chrono::seconds(rank * 2));
        // Получаем информацию о пришедшей матрице, в том числе размер
        // Выделяем память и получаем её
        MPI_Status status_x;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status_x);
        MPI_Get_count(&status_x, MPI_INT, &n);
        printf("Я - процесс %d. Получил матрицу %dx%d:", rank, (int) sqrt(n), (int) sqrt(n));
        int* a = (int*)malloc(sizeof(int ) * n);
        int x[n];
        MPI_Recv(a, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        int diag_index = 0;
        //   Пришедшие данные представлены в виде линейного массива. Данный алгоритм позволяет найти
        // диагональные элементы, не прибегая к созданию матрицы
        for(int i = 0; i < n; i++){
            if(i % (int) sqrt(n) == 0){
                printf("\n");
                x[diag_index] = a[diag_index + diag_index * (int) sqrt(n)];
                diag_index++;
            }
            printf("%5d", a[i]);
        }
        printf("\nВозвращаю диагональ...");
        printf("\n----------------------------------------------------------------------------------------\n");
        // Отправляем данные обратно
        MPI_Send(x, (int) sqrt(n), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
