#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#define NUM_EXPLORERS 2
#define NUM_MINERS 3
#define NUM_TRANSPORTERS 3

sem_t explorer_sem, miner_sem, transporter_sem, mining_sem,transporting_sem,exploring_sem;
pthread_t explorers[NUM_EXPLORERS], miners[NUM_MINERS], transporters[NUM_TRANSPORTERS];
int idle_miners= NUM_MINERS, idle_transporters=NUM_TRANSPORTERS;
pthread_mutex_t miner_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t transporter_mutex = PTHREAD_MUTEX_INITIALIZER;

// Signal handler for CTRL+C
void signal_handler(int signum) {
    printf("\n\033[1;97m======================================\033[0m\n");
    printf("\n\033[1;97mReceived CTRL+C. Cancel all thread...\033[0m\n");
    // Cancel all threads
    for (int i = 0; i < NUM_EXPLORERS; i++){
        pthread_cancel(explorers[i]);
	printf("\033[1;97mCancelling Explorer %d\033[0m\n", i+1);
    }
    for (int i = 0; i < NUM_MINERS; i++){
        pthread_cancel(miners[i]);
	printf("\033[1;97mCancelling Miner %d\033[0m\n", i+1);
    }
    for (int i = 0; i < NUM_TRANSPORTERS; i++){
        pthread_cancel(transporters[i]);
	printf("\033[1;97mCancelling Transporter %d\033[0m\n", i+1);
    }
    // Exit
    printf("\n\033[1;97mTerminated..\033[0m\n");
    exit(0);
}

// Explorer thread function
void *explorer(void *arg) {
    int id = *((int *)arg);
    while (1) {
    
	sem_wait(&explorer_sem);
        printf("\033[1;96mExplorer-%d: exploring...\033[0m\n", id);
        sleep(rand() % 3 + 2); // Simulate exploration time
        sem_post(&miner_sem);
        sem_wait(&explorer_sem);
        sem_post(&transporting_sem);
        sem_wait(&exploring_sem);
        sem_post(&explorer_sem);
        
        printf("\033[1;96mExplorer-%d: target found!\033[0m\n", id);
        sem_post(&mining_sem);
 

        
    }
    return NULL;
}

// Miner thread function
void *miner(void *arg) {
    int id = *((int *)arg);
    while (1) {
    
        sem_wait(&miner_sem); 
        sleep(0.5);
        printf("\033[1;93mMiner-%d: waiting...\033[0m\n", id);
        sem_post(&explorer_sem);
        sem_wait(&mining_sem);
        idle_miners--;
        printf("\033[1;93mMiner-%d: mining...          Idle Miner:%d\033[0m\n", id, idle_miners);
        sleep(rand() % 4 + 2); // Simulate extraction time
        printf("\033[1;93mMiner-%d: mined quantity = %d    Idle Miner:%d\033[0m\n", id, rand() % 5 + 1, idle_miners);
        idle_miners++;
        sem_post(&transporter_sem);
        sem_post(&explorer_sem);
       
        
        
    }
    return NULL;
}

// Transporter thread function
void *transporter(void *arg) {
    int id = *((int *)arg);
    while (1) {
   	sleep(2);
        sem_wait(&transporting_sem);
        printf("\033[1;95mTransporter-%d: waiting...\033[0m\n", id);
        idle_transporters--;
        sem_post(&exploring_sem);
        sem_wait(&transporter_sem);
        printf("\033[1;95mTransporter-%d: transporting...    Idle transporter: %d\033[0m\n", id, idle_transporters);
        sleep(4); // Simulate transport time
        idle_transporters++;
        sem_post(&explorer_sem);
        

        
    }
    return NULL;
}

int main() {
    // Initialize semaphores
    if (sem_init(&explorer_sem, 0, 1) != 0 || sem_init(&miner_sem, 0, 3) != 0 || sem_init(&transporter_sem, 0, 0) != 0|| sem_init(&mining_sem, 0, 0) != 0|| sem_init(&transporting_sem, 0, 3) != 0|| sem_init(&exploring_sem, 0, 0) != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    // Register signal handler for CTRL+C
    signal(SIGINT, signal_handler);

    // Create explorer threads
    for (int i = 0; i < NUM_EXPLORERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&explorers[i], NULL, explorer, id) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Create miner threads
    for (int i = 0; i < NUM_MINERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&miners[i], NULL, miner, id) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Create transporter threads
    for (int i = 0; i < NUM_TRANSPORTERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&transporters[i], NULL, transporter, id) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish (should never happen)
    for (int i = 0; i < NUM_EXPLORERS; i++)
        pthread_join(explorers[i], NULL);
    for (int i = 0; i < NUM_MINERS; i++)
        pthread_join(miners[i], NULL);
    for (int i = 0; i < NUM_TRANSPORTERS; i++)
        pthread_join(transporters[i], NULL);

    // Destroy semaphores
    sem_destroy(&explorer_sem);
    sem_destroy(&miner_sem);
    sem_destroy(&transporter_sem);
    sem_destroy(&transporting_sem);
    sem_destroy(&mining_sem);
    sem_destroy(&exploring_sem);

    return 0;
}
