#include <stdio.h>

#define X 15
#define Y 25

void PrintLine(int arr[]) {
    int i;
    for (i = 0; i < Y; i++) {
        switch (arr[i]) {
            case 0: case 6:
                printf("\033[0;0m      ");
                break;
            case 1: case 2: case 3: case 4: case 5:
                printf("\033[44m      ");                   // B
                break;
            case 10: case 11: case 12: case 13: case 14:
            case 15: case 16: case 17: case 18: case 19:
                printf("\033[42m      ");                   // G
                break;
            case 20: case 21: case 22: case 23: case 24:
            case 25: case 26: case 27: case 28: case 29:
                printf("\033[41m      ");                   // R
                break;
        }
    }
    printf("\n");
    
    for (i = 0; i < Y; i++) {
        switch (arr[i]) {
            case 0:
                printf("\033[0;0m      ");
                break;
            case 6:
                printf("\033[0;0m   *  ");
                break;
            case 1: case 2: case 3: case 4: case 5:
                printf("\033[44m  \033[0;0mB%d\033[44m  ", arr[i] % 10);
                break;
            case 10: case 11: case 12: case 13: case 14:
            case 15: case 16: case 17: case 18: case 19:
                printf("\033[42m  \033[0;0mG%d\033[42m  ", arr[i] % 10);
                break;
            case 20: case 21: case 22: case 23: case 24:
            case 25: case 26: case 27: case 28: case 29:
                printf("\033[41m  \033[0;0mR%d\033[41m  ", arr[i] % 10);
                break;
        }
    }
    printf("\n");

    for (i = 0; i < Y; i++) {
        switch (arr[i]) {
            case 0: case 6:
                printf("\033[0;0m      ");
                break;
            case 1: case 2: case 3: case 4: case 5:
                printf("\033[44m      ");
                break;
            case 10: case 11: case 12: case 13: case 14:
            case 15: case 16: case 17: case 18: case 19:
                printf("\033[42m      ");
                break;
            case 20: case 21: case 22: case 23: case 24:
            case 25: case 26: case 27: case 28: case 29:
                printf("\033[41m      ");
                break;
        }
    }
    printf("\n");
}

int main(void) {
    int board[X][Y] = { [X / 2 - 2][Y / 2    ] = 1,                         
                        [X / 2 - 1][Y / 2    ] = 2,                         
                        [X / 2    ][Y / 2    ] = 3,                          
                        [X / 2 + 1][Y / 2    ] = 4,                         
                        [X / 2 + 2][Y / 2    ] = 5 };


    int row;
    system("cls");
    for (row = 0; row < X; row++) {
        PrintLine(board[row]);
    }

    printf("\033[0;0m");
    printf("A");

    return 0;
}