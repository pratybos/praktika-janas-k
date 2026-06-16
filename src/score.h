#pragma once

#include "defines.h"

struct Score {
    int green;
    int red;
};

typedef enum {
    CHECK_SCORE,
    DIM_UNSCORING_TILES
} Score_Action;

struct Score CheckScore(int arr[MAX][MAX], Score_Action game_finished);