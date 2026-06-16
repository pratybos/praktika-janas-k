#include "score.h"
#include "board.h"
#include "animation.h"

struct Score CheckScore(int arr[MAX][MAX], Score_Action action) {

    struct Score score;
    score.green = 0;
    score.red = 0;
    
    for (int row = bounds.row_min; row <= bounds.row_max; row++) {
        for (int col = bounds.col_min; col <= bounds.col_max; col++) {
            if (arr[row + 1][col] == BRANCH || 
                arr[row - 1][col] == BRANCH || 
                arr[row][col + 1] == BRANCH || 
                arr[row][col - 1] == BRANCH) {
                    if (arr[row][col] == GREEN_LEAF)
                        score.green += 1;
                    else if (arr[row][col] == RED_LEAF)
                        score.red += 1;
            }
            else if (action == DIM_UNSCORING_TILES && arr[row][col] != BRANCH) {
                animation[row][col].tile_tint = GRAY;
            }
        }
    }

    return score;        
}