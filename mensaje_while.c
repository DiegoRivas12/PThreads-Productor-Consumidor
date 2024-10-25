#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_MAX 100
#define THREAD_COUNT 4

char* messages[THREAD_COUNT];  // Matriz compartida de mensajes

// Función que ejecutará cada hilo
void* Send_msg(void* rank) {
    long my_rank = (long) rank;
    long dest = (my_rank + 1) % THREAD_COUNT;      // Determina el destinatario del mensaje
    long source = (my_rank + THREAD_COUNT - 1) % THREAD_COUNT; // Determina el remitente esperado

    // Reservar memoria para el mensaje y crearlo
    char* my_msg = malloc(MSG_MAX * sizeof(char));
    sprintf(my_msg, "Hello to %ld from %ld", dest, my_rank);
    
    // Colocar el mensaje en la matriz compartida
    messages[dest] = my_msg;

    // Intentar leer el mensaje asignado a este hilo
    while (messages[my_rank] == NULL);
    //printf("Thread %ld > %s\n", my_rank, messages[my_rank]);

    if (messages[my_rank] != NULL)
        printf("Thread %ld > %s\n", my_rank, messages[my_rank]);
    else
        printf("Thread %ld > No message from %ld\n", my_rank, source);

    return NULL;
}

int main() {
    long thread;
    pthread_t threads[THREAD_COUNT];

    // Inicializar la matriz de mensajes a NULL
    for (thread = 0; thread < THREAD_COUNT; thread++) {
        messages[thread] = NULL;
    }

    // Crear los hilos
    for (thread = 0; thread < THREAD_COUNT; thread++) {
        pthread_create(&threads[thread], NULL, Send_msg, (void*) thread);
    }

    // Esperar a que los hilos terminen
    for (thread = 0; thread < THREAD_COUNT; thread++) {
        pthread_join(threads[thread], NULL);
    }

    // Liberar la memoria de los mensajes
    for (thread = 0; thread < THREAD_COUNT; thread++) {
        if (messages[thread] != NULL) {
            free(messages[thread]);
        }
    }

    return 0;
}
