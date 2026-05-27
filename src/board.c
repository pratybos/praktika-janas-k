#include "raylib.h"
#include "defines.h"
#include "board.h"
#include "animation.h"

struct Board_Bounds InitBoard(int arr[MAX][MAX], int type) {
    struct Board_Bounds bounds;

    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            arr[row][col] = 0;
        }
    }
    bounds.row_min = bounds.row_max = bounds.col_min = bounds.col_max = CENTER;

    switch (type) {
        case 1:
            // Vertical line start
            bounds.row_min = CENTER - BRANCH_AMOUNT / 2;
            bounds.row_max = CENTER + BRANCH_AMOUNT / 2;
            bounds.col_min = bounds.col_max = CENTER;  

            for (int i = bounds.row_min; i <= bounds.row_max; i++) {
                arr[i][CENTER] = BRANCH;
            }
            break;

        case 2:
            // Horizontal start
            bounds.col_min = CENTER - BRANCH_AMOUNT / 2;
            bounds.col_max = CENTER + BRANCH_AMOUNT / 2;
            bounds.row_min = bounds.row_max = CENTER;  

            for (int i = bounds.col_min; i <= bounds.col_max; i++) {
                arr[CENTER][i] = BRANCH;
            }
            break;

        case 3:
            // Plus start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        bounds.col_min--;
                        arr[CENTER][bounds.col_min] = BRANCH;
                        break;
                    case 2:         // UP
                        bounds.row_min--;
                        arr[bounds.row_min][CENTER] = BRANCH;
                        break;
                    case 3:         // RIGHT
                        bounds.col_max++;
                        arr[CENTER][bounds.col_max] = BRANCH;
                        break;
                    case 0:         // DOWN
                        bounds.row_max++;
                        arr[bounds.row_max][CENTER] = BRANCH;
                        break;
                }
            }
            break;

        case 4:
            // V start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        bounds.col_min--;
                        arr[bounds.row_min][bounds.col_min] = BRANCH;
                        break;
                    case 2:         // Right
                        bounds.col_max++;
                        arr[bounds.row_min][bounds.col_max] = BRANCH;
                        break;
                    case 3:         // LEFT-UP
                        bounds.row_min--;
                        arr[bounds.row_min][bounds.col_min] = BRANCH;
                        break;
                    case 0:         // RIGHT-UP
                        arr[bounds.row_min][bounds.col_max] = BRANCH;
                        break;
                }
            }
            break;
    }

    int grid_width = (bounds.col_max - bounds.col_min + 3) * TILE_SIZE;
    int grid_height = (bounds.row_max - bounds.row_min + 3) * TILE_SIZE; 
    bounds.grid_origin.x = (SCREEN_WIDTH - grid_width) / 2;
    bounds.grid_origin.y = (SCREEN_HEIGHT - grid_height) / 2;

    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            animation[row][col].tile_tint = WHITE;
        }
    }

    return bounds;
}

void AddArrows(int arr[MAX][MAX], int mode) {

        // Clear arrows
        for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++)
            for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) 
                if (arr[row][col] == ARROW_LEFT || 
                    arr[row][col] == ARROW_RIGHT || 
                    arr[row][col] == ARROW_UP || 
                    arr[row][col] == ARROW_DOWN )
                        arr[row][col] = 0;
        
        if (mode == ARROWS_NONE)
            return;

        for (int i = bounds.col_min; i <= bounds.col_max; i++) {
            if (mode == ARROW_DOWN || mode == ARROWS_ALL)
                arr[bounds.row_min - 1][i] = ARROW_DOWN; 
            if (mode == ARROW_UP || mode == ARROWS_ALL)
                arr[bounds.row_max + 1][i] = ARROW_UP;
        }

        for (int i = bounds.row_min; i <= bounds.row_max; i++) {
            if (mode == ARROW_LEFT || mode == ARROWS_ALL) 
                arr[i][bounds.col_max + 1] = ARROW_LEFT;
            if (mode == ARROW_RIGHT || mode == ARROWS_ALL)
                arr[i][bounds.col_min - 1] = ARROW_RIGHT;
        }        
}