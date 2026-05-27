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

extern struct Board_Bounds bounds;

struct Board_Bounds InitBoard(int arr[MAX][MAX], int type);
void AddArrows(int arr[MAX][MAX], int mode);