#include <stdio.h>
#include <windows.h>

#define X 15
#define Y 25

void PrintBoard(int arr[X][Y]);
void CheckPositions(int arr[X][Y]);


void PrintBoard(int arr[X][Y]) {
    CheckPositions(arr);
    int i;
    system("cls");

    for (int row = 0; row < X; row++) {
        for (i = 0; i < Y; i++) {
            switch (arr[row][i]) {
                case 0: case 1: case 2: case 3:
                    printf("\033[0;0m      ");
                    break;
                case 31: case 32: case 33: case 34: case 35:
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
            switch (arr[row][i]) {
                case 0:
                    printf("\033[0;0m      ");
                    break;
                case 1:
                    printf("\033[0;0m  __  ");
                    break;
                case 2:
                    printf("\033[0;0m   |  ");
                    break;
                case 3:
                    printf("\033[0;0m   +  ");
                    break;
                case 31: case 32: case 33: case 34: case 35:
                    printf("\033[44m  \033[0;0mB%d\033[44m  ", arr[row][i] % 10);
                    break;
                case 10: case 11: case 12: case 13: case 14:
                case 15: case 16: case 17: case 18: case 19:
                    printf("\033[42m  \033[0;0mG%d\033[42m  ", arr[row][i] % 10);
                    break;
                case 20: case 21: case 22: case 23: case 24:
                case 25: case 26: case 27: case 28: case 29:
                    printf("\033[41m  \033[0;0mR%d\033[41m  ", arr[row][i] % 10);
                    break;
            }
        }
        printf("\n");

        for (i = 0; i < Y; i++) {
            switch (arr[row][i]) {
                case 0: case 1: case 2: case 3:
                    printf("\033[0;0m      ");
                    break;
                case 31: case 32: case 33: case 34: case 35:
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
}

void CheckPositions(int arr[X][Y]) {
    int row, col;
    int lastTileInRow[Y] = {0};
    for (row = 0; row < X; row++) {
        int lastTileInCol = 0;
        for (col = 0; col < Y; col++) {     
            if (arr[row][col] < 4)
                arr[row][col] = 0;

            if (arr[row][col] > 3) {        // if not empty and not potential symbol                
                if (lastTileInRow[col] == 0 || lastTileInRow[col] == row) {
                    lastTileInRow[col] = row + 1;       // last non-empty row num + 1 (if 0 row is not empty)
                }
                else {                                                      // detected gap
                    for (int i = row - 1; i > (lastTileInRow[col] - 1); i--) {  // reset gap positions
                        arr[i][col] -= 2;                        
                    }
                    lastTileInRow[col] = row + 1;
                }

                if (lastTileInCol == 0 || lastTileInCol == col) {
                    lastTileInCol = col + 1;
                }
                else {
                    for (int i = col - 1; i > (lastTileInCol - 1); i--) {
                        arr[row][i] -= 1;                        
                    }
                    lastTileInCol = col + 1;
                }
                
               continue;
            }
                
            if (col != 0 && arr[row][col - 1] > 3 ||        // not empty or *
                col != Y - 1 && arr[row][col + 1] > 3) {
                    arr[row][col] += 1;                     // if '1' horizontal -, '2' vertical |, 3 both +
                }
            if (row != 0 && arr[row - 1][col] > 3     ||
                row != X - 1 && arr[row + 1][col] > 3   ) {
                    arr[row][col] += 2;
            }                     
        }
    }

}

int main(void) {
    int board[X][Y] = { 
                        [X / 2 - 2][Y / 2    ] = 31, 
                        [X / 2 - 1][Y / 2    ] = 32, 
                        [X / 2    ][Y / 2    ] = 33, 
                        [X / 2 + 1][Y / 2    ] = 34, 
                        [X / 2 + 2][Y / 2    ] = 35
                    };

    PrintBoard(board);
    
    printf("\033[0;0m");
    printf("A");

    return 0;
}