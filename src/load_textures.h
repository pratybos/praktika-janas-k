#pragma once
#include "raylib.h"

struct Games {
    Texture2D branch;
    Texture2D green_leaf;
    Texture2D red_leaf;

    Texture2D mini_green_leaf;
    Texture2D mini_red_leaf;

    Texture2D arrow_up;
    Texture2D arrow_right;
    Texture2D arrow_down;
    Texture2D arrow_left;

    Texture2D arrow_f_up;
    Texture2D arrow_f_right;
    Texture2D arrow_f_down;
    Texture2D arrow_f_left;

    Texture2D main_title;
    Texture2D background;
};

extern struct Games textures;

void LoadTextures(void);
void UnloadTextures(void);