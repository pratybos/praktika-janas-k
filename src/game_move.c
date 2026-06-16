#include "game_move.h"
#include "defines.h"
#include "board.h"
#include "animation.h"

struct Board_Bounds GameMove(int arr[MAX][MAX], struct Turn_Move move, Turn_Type mode) {
    // Tile pushing based on direction
    int i, j;
    switch (move.direction) {

        case ARROW_LEFT:
            i = bounds.col_max;
            while (arr[move.row][i] == EMPTY_SPACE || arr[move.row][i] > 3)
                i--;

            j = i - 1;
            while (arr[move.row][j] == RED_LEAF || arr[move.row][j] == GREEN_LEAF || arr[move.row][j] == BRANCH)
                j--;

            if (j < bounds.col_min && mode == PERMANENT) {
                bounds.col_min--;
                bounds.grid_origin.x -= TILE_SIZE;
            }

            for (; j <= i; j++) {
                if (mode == PERMANENT) {
                    animation[move.row][j].push_offset = TILE_SIZE;
                    animation[move.row][j].push_direction.x = 1;
                    animation[move.row][j].push_direction.y = 0;
                }
                arr[move.row][j] = arr[move.row][j + 1];
            }
            
            arr[move.row][i] = move.color;
            if (mode == PERMANENT) {
                animation[move.row][i].push_offset = TILE_SIZE;
                animation[move.row][i].push_direction.x = 1;
                animation[move.row][i].push_direction.y = 0;
            }
            break;

        case ARROW_RIGHT:
            i = bounds.col_min;
            while (arr[move.row][i] == EMPTY_SPACE || arr[move.row][i] > 3)
                i++;

            j = i + 1;
            while (arr[move.row][j] == RED_LEAF || arr[move.row][j] == GREEN_LEAF || arr[move.row][j] == BRANCH)
                j++;

            if (j > bounds.col_max && mode == PERMANENT) {
                bounds.col_max++;
            }

            for (; j >= i; j--) {
                if (mode == PERMANENT) {
                    animation[move.row][j].push_offset = TILE_SIZE;
                    animation[move.row][j].push_direction.x = -1;
                    animation[move.row][j].push_direction.y = 0;
                }
                arr[move.row][j] = arr[move.row][j - 1];
            }

            arr[move.row][i] = move.color;
            if (mode == PERMANENT) {
                animation[move.row][i].push_offset = TILE_SIZE;
                animation[move.row][i].push_direction.x = -1;
                animation[move.row][i].push_direction.y = 0;
            }
            break;
            
        case ARROW_UP:
            i = bounds.row_max;
            while (arr[i][move.col] == EMPTY_SPACE || arr[i][move.col] > 3)
                i--;

            j = i - 1;
            while (arr[j][move.col] == RED_LEAF || arr[j][move.col] == GREEN_LEAF || arr[j][move.col]== BRANCH)
                j--;

            if (j < bounds.row_min && mode == PERMANENT) {
                bounds.row_min--;
                bounds.grid_origin.y -= TILE_SIZE;
            }

            for (; j <= i; j++) {
                if (mode == PERMANENT) {
                    animation[j][move.col].push_offset = TILE_SIZE;
                    animation[j][move.col].push_direction.x = 0;
                    animation[j][move.col].push_direction.y = 1;
                }
                arr[j][move.col]= arr[j + 1][move.col];
            }
            
            arr[i][move.col] = move.color;
            if (mode == PERMANENT) {
                animation[i][move.col].push_offset = TILE_SIZE;
                animation[i][move.col].push_direction.x = 0;
                animation[i][move.col].push_direction.y = 1;
            }
            break;

        case ARROW_DOWN:
            i = bounds.row_min;
            while (arr[i][move.col] == EMPTY_SPACE || arr[i][move.col] > 3)
                i++;

            j = i + 1;
            while (arr[j][move.col] == RED_LEAF || arr[j][move.col] == GREEN_LEAF || arr[j][move.col]== BRANCH)
                j++;

            if (j > bounds.row_max && mode == PERMANENT) {
                bounds.row_max++;
            }

            for (; j >= i; j--) {
                arr[j][move.col]= arr[j - 1][move.col];
                if (mode == PERMANENT) {
                    animation[j][move.col].push_offset = TILE_SIZE;
                    animation[j][move.col].push_direction.x = 0;
                    animation[j][move.col].push_direction.y = -1;
                }
            }
            
            arr[i][move.col] = move.color;
            if (mode == PERMANENT) {
                animation[i][move.col].push_offset = TILE_SIZE;
                animation[i][move.col].push_direction.x = 0;
                animation[i][move.col].push_direction.y = -1;
            }
            break;
    }

    return bounds;
}