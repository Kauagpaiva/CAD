#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Função para trocar dois elementos
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Verifica se o vetor está ordenado
int is_sorted(int *arr, int n) {
    for (int i = 0; i < n - 1; i++)
        if (arr[i] > arr[i + 1]) return 0;
    return 1;
}

// Realiza a ordenação local (dentro de cada processo)
void local_sort(int *local, int local_n, int phase) {
    int start = (phase % 2 == 0) ? 1 : 0;
    for (int i = start; i < local_n - 1; i += 2) {
        if (local[i] > local[i + 1]) {
            swap(&local[i], &local[i + 1]);
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);  // Inicializa o ambiente MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Identificador do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Total de processos

    if (argc != 2) {
        if (rank == 0)
            printf("Uso: %s <tamanho_array>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);           // Tamanho total do vetor
    int local_n = n / size;          // Cada processo lida com uma parte
    int *local = malloc(local_n * sizeof(int));
    int *global = NULL;

    if (rank == 0) {
        // Só o processo 0 aloca e inicializa o vetor global
        global = malloc(n * sizeof(int));
        srand(42);
        for (int i = 0; i < n; i++)
            global[i] = rand() % 1000;
    }

    // Distribui partes do vetor entre os processos
    MPI_Scatter(global, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);  // Sincroniza os processos antes da medição
    double total_start = MPI_Wtime();
    double comm_time = 0.0;

    for (int phase = 0; phase < n; phase++) {
        local_sort(local, local_n, phase);

        // Determina o vizinho
        int neighbor = -1;
        if (phase % 2 == 0) {
            if (rank % 2 == 0 && rank + 1 < size) neighbor = rank + 1;
            else if (rank % 2 != 0) neighbor = rank - 1;
        } else {
            if (rank % 2 != 0 && rank + 1 < size) neighbor = rank + 1;
            else if (rank % 2 == 0 && rank != 0) neighbor = rank - 1;
        }

        if (neighbor >= 0 && neighbor < size) {
            int send_val = (rank < neighbor) ? local[local_n - 1] : local[0];
            int recv_val;
            double comm_start = MPI_Wtime();
            // Troca de bordas entre vizinhos
            MPI_Sendrecv(&send_val, 1, MPI_INT, neighbor, 0,
                         &recv_val, 1, MPI_INT, neighbor, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            double comm_end = MPI_Wtime();
            comm_time += comm_end - comm_start;

            // Atualiza extremos locais após comunicação
            if (rank < neighbor && send_val > recv_val)
                local[local_n - 1] = recv_val;
            else if (rank > neighbor && recv_val > send_val)
                local[0] = recv_val;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double total_end = MPI_Wtime();
    double local_total_time = total_end - total_start;

    // Coleta os tempos em cada processo
    double global_total_time, global_comm_time;
    MPI_Reduce(&local_total_time, &global_total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&comm_time, &global_comm_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Junta o vetor final no processo 0
    MPI_Gather(local, local_n, MPI_INT, global, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Array está ordenado: %s\n", is_sorted(global, n) ? "Sim" : "Não");
        printf("Tempo Total: %.6f s\n", global_total_time);
        printf("Tempo Comunicação: %.6f s\n", global_comm_time);
        printf("Overhead: %.2f%%\n", (global_comm_time / global_total_time) * 100);
        free(global);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
