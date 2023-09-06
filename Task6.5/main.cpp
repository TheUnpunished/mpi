#include <iostream>
#include "mpi.h"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    // Запускать исключительно с 2 потоками!
    using namespace std;
    using namespace std::chrono;
    MPI_Init(&argc, &argv);
    int rank;
    int n;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Аналогично пункту а)
    for(int i = 1; i < 902; i+=100) {
        MPI_Request request;
        n = 10 * i * i;
        char message[n];
        for (int i = 0; i < n; i++) {
            message[i] = (char) i;
        }
        if(rank == 0){
            auto start = high_resolution_clock::now();
            // Отправляем сообщение без блокировки, чтобы обеспечить одновременность передачи
            MPI_Isend(message, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &request);
            MPI_Recv(message, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            printf("Процесс %d: n = %d, ", rank, n);
            cout << "t = "
                 << std::chrono::duration_cast<microseconds>
                         (high_resolution_clock::now() - start).count() << " микросекунд" << endl;
        }
        else{
            // Отправляем сообщение без блокировки, чтобы обеспечить одновременность передачи
            auto start = high_resolution_clock::now();
            MPI_Isend(message, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
            MPI_Recv(message, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            printf("Процесс %d: n = %d, ", rank, n);
            cout << "t = "
                 << std::chrono::duration_cast<microseconds>
                         (high_resolution_clock::now() - start).count() << " микросекунд" << endl;
        }
        MPI_Wait(&request, MPI_STATUSES_IGNORE);
        this_thread::sleep_for(seconds(1));
    }
    MPI_Finalize();
    return 0;
}
