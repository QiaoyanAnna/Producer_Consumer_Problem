#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "tands.h"

sem_t m;
sem_t empty;
sem_t full;
int* queue;
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
// void summary(int work, int ask, int receive, int complete, int sleep, int threads[], int nthreads);

int main(int argc, char** argv) {

    char outputFileName[15];
    int nthreads;
    char* id = "0";

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
    int n;
    int work = 0;
    int sleep = 0;
    int sizeOfQueue = nthreads * 2;
    queue = (int*)malloc(sizeOfQueue);
    head = 0;
    tail = 0;
    eof = false;

    struct Thread *threads;
    threads = (struct Thread *)malloc(sizeof(struct Thread *) * nthreads);

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

    for (int i = 0; i < nthreads; i++) {
        threads[i].consumerId = i+1;
        if (pthread_create(&threads[i].tid, NULL, removeWork, &threads[i])){
            perror("Error occured during creating a thread\n");
            return -1;
        }
    }

    
    while (1){
        if (scanf("%c%d", &request, &n) == EOF){            
            eof = true;
            printf("time to break\n");
            break;
        }
        if (request == 'T') {
            // put work into queue
            sem_wait(&empty);
            sem_wait(&m);
            queue[tail] = n;
            tail++;
            // for (int i=0; i<tail-head; i++) {
            //     printf("parent: %d, ", queue[i]);
            // }
            sem_post(&m);
            sem_post(&full);



            work++;

            // Trans(n);
        } else if (request == 'S') {
            sleep++;
            Sleep(n);
        } 

        getchar(); // read a newline char

    }

    for (int j = 0; j < nthreads; j++) {
        // if (pthread_join(threads[j].tid, NULL)==0){
        //     printf("joined\n");
        // }
        pthread_join(threads[j].tid, NULL);
        printf("joined\n");
    }

    // free(threads);
    // free(queue);
    // pthread_exit(0);

    // summary(work, ask, receive, complete, sleep, threads, nthreads);

    
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
    printf("created\n");
    struct Thread *threads = arg;
    int n;
    // threads->ask = threads->ask + 1;
    // printf("ThreadId: %lu, Ask: %d\n", threads->tid, threads->ask);
    while(1) {
        // sleep(5);
        if (eof && (head == tail)) {
            break;
        }
        sem_wait(&full);
        sem_wait(&m);
        n = queue[head];
        printf("item: %d\n", n);
        head++;

        // for (int i=0; i<tail-head; i++) {
        //     printf("child: %d, ", queue[i]);
        // }

        sem_post(&m);
        sem_post(&empty);
        //call tran

    }



    

    return 0;
}


void summary(int work, int ask, int receive, int complete, int sleep, int nthreads) {
    fprintf(stdout, "Summary:\n");
    fprintf(stdout, "\tWork\t\t%d\n", work);
    fprintf(stdout, "\tAsk\t\t%d\n", ask);
    fprintf(stdout, "\tReceive\t\t%d\n", receive);
    fprintf(stdout, "\tComplete\t%d\n", complete);
    fprintf(stdout, "\tSleep\t\t%d\n", sleep);
    // for (int i = 0; i < nthreads; i++) {
    //     fprintf(stdout, "\tThread %d\t%d\n", i+1, threads[i]); caution: threads
    // }
}
