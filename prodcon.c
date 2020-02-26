#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tands.h"

bool isNum(char* num);
void summary(int work, int ask, int receive, int complete, int sleep, char threads[], int nthreads);

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
    int ask = 0;
    int receive = 0;
    int complete = 0;
    int sleep = 0;
    char threads[nthreads];

    memset(threads, 0, nthreads);  

    while (1){
        if (scanf("%c%d", &request, &n) == EOF){
            break;
        }

        if (request == 'T') {
            work++;
            Trans(n);
        } else if (request == 'S') {
            sleep++;
            Sleep(n);
        } 

        getchar(); // read a newline char

    }

    summary(work, ask, receive, complete, sleep, threads, nthreads);

    
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

void summary(int work, int ask, int receive, int complete, int sleep, char threads[], int nthreads) {
    fprintf(stdout, "Summary:\n");
    fprintf(stdout, "\tWork\t\t%d\n", work);
    fprintf(stdout, "\tAsk\t\t%d\n", ask);
    fprintf(stdout, "\tReceive\t\t%d\n", receive);
    fprintf(stdout, "\tComplete\t%d\n", complete);
    fprintf(stdout, "\tSleep\t\t%d\n", sleep);
    for (int i = 0; i < nthreads; i++) {
        fprintf(stdout, "\tThread %d\t%d\n", i+1, threads[i]);
    }
}
