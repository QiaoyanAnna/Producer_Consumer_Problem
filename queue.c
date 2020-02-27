#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void printQueue (int queue[], int head, int tail) {
    for (int i = head; i < tail; i++) {
        printf("%d ", queue[i]);
    }
    printf("\n");
}

void enqueue (int queue[], int* head, int* tail, int sizeOfQueue, int n ) {
    if ((*tail-*head) != sizeOfQueue) { // not full
        queue[*tail] = n;
        (*tail)++;
    }

//     printf("enqueue: %d, tail: %d", queue, tail);
}

int dequeue (int queue[], int* head, int* tail) {
    if (*head != *tail) { // not empty
        int item = queue[*head];
        (*head)++;
        return item;
    }
    return -1;
}


 
int main()
{

    // int n = 1;
    int head = 0;
    int tail = 0;
    int sizeOfQueue = 5;
    int queue[sizeOfQueue];
    int item;

    for (int i = 0; i < 6; i++) {
        enqueue(queue, &head, &tail, sizeOfQueue, i);
        printQueue(queue, head, tail);
    }
    
    for (int i = 0; i < 6; i++) {
        item = dequeue(queue, &head, &tail);
        printf("item: %d\n", item);
        printQueue(queue, head, tail);
    }
    
    
    printf("Result: \n");
    printQueue(queue, head, tail);
    // printf("main: %s", queue);
    return 0;
}