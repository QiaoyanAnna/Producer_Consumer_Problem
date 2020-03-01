#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#include "tands.h"

sem_t m;
sem_t empty;
sem_t full;
clock_t begin;
int *queue;
int sizeOfQueue;
int head;
int tail;
bool eof;

struct Thread {
    pthread_t tid;
    int consumerId;
    int ask;
    int receive;
    int complete;
};

bool isNum(char* num);
void *removeWork(void* arg);
void summary(int work, int sleep, clock_t end, struct Thread *threads, int nthreads);

int main(int argc, char** argv) {
    begin = clock();
    char outputFileName[15];
    int nthreads;
    char *id = "0";

    // check the input if valid
    if (argc == 2 || argc == 3) {
        if (!isNum(argv[1])) {
            fprintf(stderr, "Invalid input.\n");
            return -1;
        }
        nthreads = atoi(argv[1]);
        if (nthreads == 0) {
            fprintf(stderr, "Number of threads must be greater than 0.\n");
            return -1;
        }
        if (argc == 3) {
            if (!isNum(argv[2])) {
                fprintf(stderr, "Invalid input.\n");
                return -1;
            }
            id = argv[2];
        }
    } else {
        fprintf(stderr, "Invalid input.\n");
        return -1;
    }

    // figure out the output file name        
    if (strcmp(id, "0") == 0) {
        strcpy(outputFileName, "prodcon.log");
    } else {
        strcpy(outputFileName, "prodcon.");
        strcat(outputFileName, argv[2]);
        strcat(outputFileName, ".log");
    }
    stdout = fopen(outputFileName, "w");

    char request;
    int n, q;
    int work = 0;
    int sleep = 0;
    clock_t current;
    double timeTaken;
    head = 0;
    tail = 0;
    eof = false;
    sizeOfQueue = nthreads * 2 + 1; // extra space for tail to move

    // semaphore m=1
    if (sem_init(&m, 0, 1)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

    // semaphore empty = sizeOfQueue - 1 = 2 * #consumers
    if (sem_init(&empty, 0, sizeOfQueue - 1)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

    // semaphore full = 0
    if (sem_init(&full, 0, 0)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

    // create threads and initialize ask, receive, complete to zero
    struct Thread *threads;
    threads = (struct Thread *)malloc(sizeof(struct Thread) * nthreads);
    for (int i = 0; i < nthreads; i++) {
        threads[i].consumerId = i+1;
        threads[i].ask = 0;
        threads[i].receive = 0;
        threads[i].complete = 0;
        if (pthread_create(&threads[i].tid, NULL, removeWork, &threads[i])){
            perror("Error occured during creating a thread\n");
            free(threads);
            return -1;
        }
    }

    // allocate space for queue to store tasks
    queue = (int *)malloc(sizeof(int) * sizeOfQueue);
    while (1){
        if (scanf("%c%d", &request, &n) == EOF){            
            current = clock();
            eof = true;
            // in order to notify customer so that the customer will not stuck in sem_wait(&full)
            sem_post(&full); 
            timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
            fprintf(stdout, "%0.3f ID= 0      End\n", timeTaken);
            break;
        }

        if (request == 'T') {
            // put work into queue
            sem_wait(&empty);
            sem_wait(&m);
            queue[tail] = n;
            // caluclate time
            current = clock();
            timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
            // if the tail reaches the end of the queue, move the tail to the beginning of the queue
            tail = (tail+1)%sizeOfQueue;
            // caluclate q
            if (tail > head){
                q = tail - head;
            } else if (head == tail) {
                q = 0; // it is not possible
            } else {
                q = sizeOfQueue - (head - tail);
            } 
            fprintf(stdout, "%0.3f ID= 0 Q=%2d Work        %2d\n", timeTaken, q, n);
            sem_post(&m);
            sem_post(&full);
            work++;
        } else if (request == 'S') {
            current = clock();
            timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
            fprintf(stdout, "%0.3f ID= 0      Sleep       %2d\n", timeTaken, n);
            Sleep(n);
            sleep++;
        } 

        getchar(); // read a newline char

    }

    // wait for all the threads to finish
    for (int j = 0; j < nthreads; j++) {
        pthread_join(threads[j].tid, NULL);
    }

    clock_t end = clock();
    summary(work, sleep, end, threads, nthreads);

    free(threads);
    free(queue);

    return 0;
}

bool isNum(char* num) {
    for (int i = 0; i < strlen(num); i++){
        if (isdigit(num[i]) == 0) {
            return false; 
        }
    }
    return true; 
} 

// consumer threads
void *removeWork(void* arg) {
    
    struct Thread *threads = arg;
    int n, q;
    double timeTaken;
    clock_t current;

    while(1) {
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        fprintf(stdout, "%0.3f ID=%2d      Ask\n", timeTaken, threads->consumerId);
        threads->ask = threads->ask + 1;

        sem_wait(&full);
        if (( head==tail ) && eof) { // complete all the task
            sem_post(&full);
            break;
        } 
        sem_wait(&m);
        n = queue[head]; // pop out the first item in the queue
        // task received
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        head = (head+1)%sizeOfQueue;
        if (tail>head) {
            q = tail - head;
        } else if (head == tail) {
            q = 0;
        } else {
            q = sizeOfQueue - (head - tail);
        }
        fprintf(stdout, "%0.3f ID=%2d Q=%2d Receive     %2d\n", timeTaken, threads->consumerId, q, n);
        threads->receive = threads->receive + 1;
        sem_post(&m);
        sem_post(&empty);
        // call Tran(n)
        Trans(n);
        // task complete
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        fprintf(stdout, "%0.3f ID=%2d      Complete    %2d\n", timeTaken, threads->consumerId, n);
        threads->complete = threads->complete + 1;
    }

    return 0;
}


void summary(int work, int sleep, clock_t end, struct Thread *threads, int nthreads) {
    int totalAsk = 0;
    int totalReceive = 0;
    int totalComplete = 0;
    double totalTime = ((double)end-begin) / CLOCKS_PER_SEC;
    double transPerSec = work / totalTime;

    // calculate the total ask, total receive and total complete
    for (int i = 0; i < nthreads; i++) {
        totalAsk = totalAsk + threads[i].ask;
        totalReceive = totalReceive + threads[i].receive;
        totalComplete = totalComplete + threads[i].complete;
    }

    // print summary
    fprintf(stdout, "Summary:\n");
    fprintf(stdout, "    Work        %2d\n", work);
    fprintf(stdout, "    Ask         %2d\n", totalAsk);
    fprintf(stdout, "    Receive     %2d\n", totalReceive);
    fprintf(stdout, "    Complete    %2d\n", totalComplete);
    fprintf(stdout, "    Sleep       %2d\n", sleep);
    for (int i = 0; i < nthreads; i++) {
        fprintf(stdout, "    Thread %2d   %2d\n", i+1, threads[i].complete);
    }
    fprintf(stdout, "Transactions per second: %0.2f\n", transPerSec);

}
