#include <iostream>
#include "mpi.h"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    // Запускать исключительно с 2 процессами!
    using namespace std;
    using namespace std::chrono;
    MPI_Init(&argc, &argv);
    int rank;
    int n;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // эксперементируем с разным количеством информации
    for(int i = 1; i < 902; i+=100){
        n = 10 * i * i;
        // 1 char = 1 байт. n байт
        char message[n];
        for (int i = 0; i < n; i++){
            message[i] = (char) i;
        }
        if((char) rank == message[0]){
            // это делается для обеспечения последовательности отправки сообщений
            message[0] = (char) 1;
            // отправляем сообщение, фиксируем время начала передачи
            auto start = high_resolution_clock::now();
            MPI_Send(message, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            // 0 поток получит сообщения последним, фиксируем время конца передачи
            MPI_Recv(message, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            printf("n = %d, ", n);
            cout << "t = "
                 << std::chrono::duration_cast<microseconds>
                         (high_resolution_clock::now() - start).count() << " микросекунд" << endl;
        } else{
            // аналогично для 1 потока
            message[0] = (char) 0;
            MPI_Recv(message, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Send(message, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
        // спим 1 секунду для читабельности
        this_thread::sleep_for(seconds(1));
    }
    MPI_Finalize();
    return 0;
}
