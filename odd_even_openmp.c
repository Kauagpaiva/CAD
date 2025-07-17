#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Função para trocar dois elementos do vetor
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Gera valores aleatórios no vetor
void generate_random_array(int *arr, int n, int max_val) {
    for (int i = 0; i < n; i++)
        arr[i] = rand() % max_val;
}

// Verifica se o vetor está ordenado
int is_sorted(int *arr, int n) {
    for (int i = 0; i < n - 1; i++)
        if (arr[i] > arr[i + 1]) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <tamanho_array> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);             // Tamanho do vetor
    int num_threads = atoi(argv[2]);  // Número de threads

    int *arr = malloc(n * sizeof(int));
    generate_random_array(arr, n, 1000);  // Gera o vetor

    double start_time = omp_get_wtime();  // Início do cronômetro

    // Algoritmo Odd-Even Sort paralelo com OpenMP
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            // Fase par
            #pragma omp parallel for num_threads(num_threads) schedule(static)
            for (int i = 1; i < n; i += 2) {
                if (arr[i - 1] > arr[i]) {
                    swap(&arr[i - 1], &arr[i]);
                }
            }
        } else {
            // Fase ímpar
            #pragma omp parallel for num_threads(num_threads) schedule(static)
            for (int i = 1; i < n - 1; i += 2) {
                if (arr[i] > arr[i + 1]) {
                    swap(&arr[i], &arr[i + 1]);
                }
            }
        }
    }

    double end_time = omp_get_wtime();  // Fim do cronômetro

    printf("Array está ordenado: %s\n", is_sorted(arr, n) ? "Sim" : "Não");
    printf("Tempo OpenMP: %.6f segundos\n", end_time - start_time);

    free(arr);  // Libera a memória
    return 0;
}
