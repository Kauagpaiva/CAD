#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// trocar dois elementos do vetor
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// algoritmo Odd-Even Transposition Sort
void odd_even_sort_serial(int arr[], int n) {
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            // compara elementos nas posições ímpares
            for (int i = 1; i < n; i += 2) {
                if (arr[i - 1] > arr[i]) {
                    swap(&arr[i - 1], &arr[i]);
                }
            }
        } else {
            //compara elementos nas posições pares
            for (int i = 1; i < n - 1; i += 2) {
                if (arr[i] > arr[i + 1]) {
                    swap(&arr[i], &arr[i + 1]);
                }
            }
        }
    }
}

// Gera um vetor de inteiros aleatórios
void generate_random_array(int arr[], int n, int max_val) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) arr[i] = rand() % max_val;
}

// imprime o vetor
void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
}

// Verifica se o vetor está ordenado
int is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_array>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]); // Tamanho do vetor
    int *arr = malloc(n * sizeof(int)); // Aloca dinamicamente o vetor


    generate_random_array(arr, n, 1000);

    if (n <= 20) {
        printf("Array original: ");
        print_array(arr, n);
    }

    clock_t start = clock();
    odd_even_sort_serial(arr, n);
    clock_t end = clock();

    if (n <= 20) {
        printf("Array ordenado: ");
        print_array(arr, n);
    }

    printf("Array está ordenado: %s\n", is_sorted(arr, n) ? "Sim" : "Não");
    printf("Tempo (Serial): %.6f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(arr); // Libera a memória alocada
    return 0;
}
