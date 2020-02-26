#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void printQueue (int queue[], int head, int tail) {
    for (int i = head; i < tail; i++) {
        printf("%d ", queue[i]);
    }
    printf("\n");
}

void enqueue (int queue[], int* tail, int sizeOfQueue, int n ) {
    if ( *tail != sizeOfQueue ) { // not full
        queue[*tail] = n;
        (*tail)++;
    }

//     printf("enqueue: %d, tail: %d", queue, tail);
}

void dequeue (int queue[], int* head, int* tail) {
    if ( *head != *tail ) { // not empty
        (*head)++;
    }
}


 
int main()
{

    int n = 1;
    int head = 0;
    int tail = 0;
    int sizeOfQueue = 5;
    int queue[sizeOfQueue];

    enqueue(queue, &tail, sizeOfQueue, n);
    enqueue(queue, &tail, sizeOfQueue, 2);
    enqueue(queue, &tail, sizeOfQueue, 3);
    enqueue(queue, &tail, sizeOfQueue, 4);
    enqueue(queue, &tail, sizeOfQueue, 5);
    printf("tail: %d\n", tail);
    printQueue(queue, head, tail);
    dequeue (queue, &head, &tail);
    printQueue(queue, head, tail);
    // printf("main: %s", queue);
    return 0;
}