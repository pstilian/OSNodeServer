#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VAL 20
#define MIN_VAL -20

void populateBoard(int board[][4]){
    //init randomizer
    srand((unsigned) time(0));

    int i, j;
    for(i = 0; i < 4; i++) for (j = 0; j < 4; j++){
        board[i][j] = (rand() % (MAX_VAL - MIN_VAL)) + MIN_VAL;
    }
}

void printBoard(int board[][4]){
    int i, j;
    for(i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            printf("%d ",board[i][j]);
        }
        printf("\n");
    }
}