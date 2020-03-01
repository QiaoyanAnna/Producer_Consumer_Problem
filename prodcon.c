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

    if (argc == 2 || argc == 3) {
        if (!isNum(argv[1])) {
            fprintf(stderr, "Invalid input.\n");
            return -1;
        }
        nthreads = atoi(argv[1]);
        if (nthreads == 0) {
            fprintf(stderr, "Number of threads need to be greater than 0.\n");
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
            
    if (strcmp(id, "0") == 0) {
        strcpy(outputFileName, "prodcon.log");
    } else {
        strcpy(outputFileName, "prodcon.");
        strcat(outputFileName, argv[2]);
        strcat(outputFileName, ".log");
    }

    // stdout = fopen(outputFileName, "w");

    char request;
    int n = 0;
    int work = 0;
    int sleep = 0;
    sizeOfQueue = nthreads * 2;
    head = 0;
    tail = 0;
    eof = false;
    clock_t current;
    double timeTaken;

    if (sem_init(&m, 0, 1)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

    if (sem_init(&empty, 0, sizeOfQueue)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

    if (sem_init(&full, 0, 0)) {
        perror("Semaphore Error\n");
        exit(-1);
    }

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

    queue = (int *)malloc(sizeof(int) * sizeOfQueue);
    while (1){
        if (scanf("%c%d", &request, &n) == EOF){            
            current = clock();
            eof = true;
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
            //tail++;
            tail = (tail+1)%sizeOfQueue;
            sem_post(&m);
            sem_post(&full);
            current = clock();
            timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
            int q;
            if (tail>head){
                q = tail - head;
            }else{
                q = sizeOfQueue - (head - tail);
            }
            fprintf(stdout, "%0.3f ID= 0 Q= %d Work\t\t%d\n", timeTaken, q, n);
            work++;
        } else if (request == 'S') {
            current = clock();
            timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
            fprintf(stdout, "%0.3f ID= 0      Sleep\t\t%d\n", timeTaken, n);
            Sleep(n);
            sleep++;
        } 

        getchar(); // read a newline char

    }

    for (int j = 0; j < nthreads; j++) {
        if (pthread_join(threads[j].tid, NULL)==0){
            printf("thread %d joined.\n", threads[j].consumerId);
        }
    }

    clock_t end = clock();
    summary(work, sleep, end, threads, nthreads);

    
    free(threads);
    free(queue);
    
    // pthread_exit(0);

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

void *removeWork(void* arg) {
    
    struct Thread *threads = arg;
    int n, q;
    double timeTaken;
    clock_t current;

    while(1) {
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        fprintf(stdout, "%0.3f ID= %d      Ask\n", timeTaken, threads->consumerId);
        threads->ask = threads->ask + 1;
        sem_wait(&full);
        if (( head==tail ) && eof) {
            sem_post(&full);
            break;
        } 
        sem_wait(&m);
        if (head != tail) {
            n = queue[head];
            //head++;
            head = (head+1)%sizeOfQueue;
            if (tail>head){
                q = tail - head;
            }else{
                q = sizeOfQueue - (head - tail);
            }
        } else {
            sem_post(&m);
            sem_post(&full);
            continue;
        }
        sem_post(&m);
        sem_post(&empty);
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        fprintf(stdout, "%0.3f ID= %d Q= %d Receive\t%d\n", timeTaken, threads->consumerId, q, n);
        threads->receive = threads->receive + 1;
        Trans(n);
        current = clock();
        timeTaken = ((double)current-begin) / CLOCKS_PER_SEC;
        fprintf(stdout, "%0.3f ID= %d      Complete\t%d\n", timeTaken, threads->consumerId, n);
        threads->complete = threads->complete + 1;
    }

    return 0;
}


void summary(int work, int sleep, clock_t end, struct Thread *threads, int nthreads) {
    int totalAsk = 0;
    int totalReceive = 0;
    int totalComplete = 0;
    double totalTime = ((double)end-begin) / CLOCKS_PER_SEC;
    printf("totalTime: %f\n", totalTime);
    double transPerSec = work / totalTime;

    for (int i = 0; i < nthreads; i++) {
        totalAsk = totalAsk + threads[i].ask;
        totalReceive = totalReceive + threads[i].receive;
        totalComplete = totalComplete + threads[i].complete;
    }

    fprintf(stdout, "Summary:\n");
    fprintf(stdout, "\tWork\t\t%d\n", work);
    fprintf(stdout, "\tAsk\t\t%d\n", totalAsk);
    fprintf(stdout, "\tReceive\t\t%d\n", totalReceive);
    fprintf(stdout, "\tComplete\t%d\n", totalComplete);
    fprintf(stdout, "\tSleep\t\t%d\n", sleep);
    for (int i = 0; i < nthreads; i++) {
        fprintf(stdout, "\tThread %d\t%d\n", i+1, threads[i].complete);
    }
    fprintf(stdout, "Transactions per second: %0.2f\n", transPerSec);

}
