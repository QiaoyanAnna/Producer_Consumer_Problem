#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


bool isNum(char* s);

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

    char request;
    int n;

    while (1){
        if (scanf("%c%d", &request, &n) == EOF){
            break;
        };
        printf("request: %c\n", request);
        printf("n: %d\n", n);
        getchar(); // for newline
    }
    
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
