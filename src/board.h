#pragma once

#include "raylib.h"
#include "defines.h"

struct Board_Bounds {
    int row_min;
    int row_max;
    int col_min;
    int col_max;
    Vector2 grid_origin;
};

typedef enum {
    VERTICAL,
    HORIZONTAL,
    PLUS,
    U,
    RANDOM
} Start_Positions;

extern struct Board_Bounds bounds;


struct Board_Bounds InitBoard(int arr[MAX][MAX], Start_Positions type);
void AddArrows(int arr[MAX][MAX], int mode);