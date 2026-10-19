#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define NUM_ITEMS 10

int buffer[TAM_BUFFER];
int in = 0;
int out = 0;

// Declaración de semáforos y mutex
sem_t empty; // Cuenta los espacios vacíos en el búfer
sem_t full;  // Cuenta los elementos producidos en el búfer
pthread_mutex_t mutex; // Protege la sección crítica del búfer

void* productor(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        // Espera si no hay espacio libre en el búfer
        sem_wait(&empty);
        
        // Entra a la sección crítica (solo un hilo modifica el búfer)
        pthread_mutex_lock(&mutex);

        buffer[in] = i;
        printf("[Productor] Producido ítem %d en la posición %d\n", i, in);
        in = (in + 1) % TAM_BUFFER;

        pthread_mutex_unlock(&mutex);
        
        // Incrementa la cuenta de ítems disponibles
        sem_post(&full);

        sleep(1); // Simula tiempo de procesamiento
    }
    pthread_exit(NULL);
}

void* consumidor(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        // Espera si no hay ítems disponibles
        sem_wait(&full);

        // Entra a la sección crítica
        pthread_mutex_lock(&mutex);

        int item = buffer[out];
        printf("  [Consumidor] Consumido ítem %d de la posición %d\n", item, out);
        out = (out + 1) % TAM_BUFFER;

        pthread_mutex_unlock(&mutex);
        
        // Incrementa la cuenta de espacios vacíos
        sem_post(&empty);

        sleep(2); // Simula tiempo de consumo
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilo_prod, hilo_cons;

    // Inicialización de semáforos:
    // sem_init(&sem, pshared, value)
    // pshared = 0 indica que se comparte entre hilos del mismo proceso
    sem_init(&empty, 0, TAM_BUFFER); // Inicia con la capacidad máxima del búfer
    sem_init(&full, 0, 0);           // Inicia con 0 ítems
    pthread_mutex_init(&mutex, NULL);

    // Creación de los hilos
    pthread_create(&hilo_prod, NULL, productor, NULL);
    pthread_create(&hilo_cons, NULL, consumidor, NULL);

    // Esperar a que ambos hilos terminen
    pthread_join(hilo_prod, NULL);
    pthread_join(hilo_cons, NULL);

    // Destrucción de semáforos y mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("\nProceso finalizado con éxito.\n");
    return 0;
}
