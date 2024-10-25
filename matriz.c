#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3               // Tamaño de la matriz (N x N)
#define NUM_THREADS 4     // Número de hilos

typedef struct {
    int **data;
    int size;
} matrix_t;

pthread_mutex_t mutex;       // Mutex para proteger la matriz de producto
matrix_t product_matrix;      // Matriz de producto compartida

// Función para asignar memoria a una matriz de tamaño NxN
matrix_t Allocate_matrix(int size) {
    matrix_t mat;
    mat.size = size;
    mat.data = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        mat.data[i] = (int *)malloc(size * sizeof(int));
    }
    return mat;
}

// Función para inicializar la matriz como la identidad
void Initialize_identity(matrix_t *mat) {
    for (int i = 0; i < mat->size; i++) {
        for (int j = 0; j < mat->size; j++) {
            mat->data[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// Función para generar valores aleatorios en una matriz
void Generate_matrix(matrix_t mat) {
    for (int i = 0; i < mat.size; i++) {
        for (int j = 0; j < mat.size; j++) {
            mat.data[i][j] = rand() % 10;  // Valores aleatorios entre 0 y 9
        }
    }
}

// Función para multiplicar dos matrices y almacenar el resultado en product_matrix
void Multiply_matrix(matrix_t *product, matrix_t mat) {
    int size = product->size;
    matrix_t temp = Allocate_matrix(size);  // Matriz temporal para almacenar el resultado

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            temp.data[i][j] = 0;
            for (int k = 0; k < size; k++) {
                temp.data[i][j] += product->data[i][k] * mat.data[k][j];
            }
        }
    }

    // Copiar el resultado en product_matrix
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            product->data[i][j] = temp.data[i][j];
        }
    }

    Free_matrix(&temp);  // Liberar la memoria de la matriz temporal
}

// Liberar memoria de la matriz
void Free_matrix(matrix_t *mat) {
    for (int i = 0; i < mat->size; i++) {
        free(mat->data[i]);
    }
    free(mat->data);
}

// Función de trabajo para cada hilo
void *Thread_work(void *rank) {//Identificador de hilo
    long my_rank = (long)rank;//representa el identificador del hilo. Así, my_rank almacena el número único de cada hilo.
    matrix_t my_mat = Allocate_matrix(N);//Aquí, my_mat se define como una matriz de tamaño N x N usando la función Allocate_matrix, que asigna memoria para una matriz de este tamaño. Esta matriz se utilizará dentro de este hilo para almacenar los datos generados aleatoriamente.

    Generate_matrix(my_mat);//Llena la matriz con valores aleatorios

    pthread_mutex_lock(&mutex);//El pthread_mutex_lock bloquea el mutex llamado mutex. Al hacer esto, el hilo asegura que solo él tenga acceso exclusivo a la product_matrix en el momento de realizar la multiplicación. 
    Multiply_matrix(&product_matrix, my_mat);//La función Multiply_matrix realiza la multiplicación entre product_matrix
    pthread_mutex_unlock(&mutex);//Después de completar la multiplicación, el hilo libera el mutex llamando a pthread_mutex_unlock. Esto permite que otros hilos adquieran el mutex y realicen sus propias operaciones en product_matrix.

    Free_matrix(&my_mat);//Este comando libera la memoria asignada a my_mat en este hilo. Esto es importante para evitar fugas de memoria, ya que my_mat ya no se necesita después de realizar la multiplicación.

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    long thread;

    // Inicializar product_matrix como identidad
    product_matrix = Allocate_matrix(N);
    Initialize_identity(&product_matrix);

    // Inicializar el mutex
    pthread_mutex_init(&mutex, NULL);

    // Crear los hilos
    for (thread = 0; thread < NUM_THREADS; thread++) {
        pthread_create(&threads[thread], NULL, Thread_work, (void *)thread);
    }

    // Esperar a que todos los hilos terminen
    for (thread = 0; thread < NUM_THREADS; thread++) {
        pthread_join(threads[thread], NULL);
    }

    // Imprimir el resultado
    printf("Matriz resultado (product_matrix):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", product_matrix.data[i][j]);
        }
        printf("\n");
    }

    // Destruir el mutex y liberar la memoria
    pthread_mutex_destroy(&mutex);
    Free_matrix(&product_matrix);

    return 0;
}
