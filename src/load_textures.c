#include "raylib.h"
#include "defines.h"
#include "load_textures.h"

void LoadTextures(void) {
    Image branch_image = LoadImage("assets/branch.png");
    Image green_leaf_image = LoadImage("assets/green.png");
    Image red_leaf_image = LoadImage("assets/red.png");

    Image mini_green_leaf_image = LoadImage("assets/green.png");
    Image mini_red_leaf_image = LoadImage("assets/red.png");

    ImageResize(&branch_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&green_leaf_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&red_leaf_image, TILE_SIZE, TILE_SIZE);

    ImageResize(&mini_green_leaf_image, TILE_SIZE / 2, TILE_SIZE / 2);
    ImageResize(&mini_red_leaf_image, TILE_SIZE / 2, TILE_SIZE / 2);

    textures.branch = LoadTextureFromImage(branch_image);
    textures.green_leaf = LoadTextureFromImage(green_leaf_image);
    textures.red_leaf = LoadTextureFromImage(red_leaf_image);

    textures.mini_green_leaf = LoadTextureFromImage(mini_green_leaf_image);
    textures.mini_red_leaf = LoadTextureFromImage(mini_red_leaf_image);

    Image arrow_image = LoadImage("assets/arrow.png");
    ImageResize(&arrow_image, TILE_SIZE, TILE_SIZE);

    textures.arrow_up = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    textures.arrow_right = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    textures.arrow_down = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    textures.arrow_left = LoadTextureFromImage(arrow_image);

    Image arrow_f_image = LoadImage("assets/arrow_f.png");
    ImageResize(&arrow_f_image, TILE_SIZE, TILE_SIZE);

    textures.arrow_f_up = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    textures.arrow_f_right = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    textures.arrow_f_down = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    textures.arrow_f_left = LoadTextureFromImage(arrow_f_image);

    Image main_title = LoadImage("assets/title.png");
    textures.main_title = LoadTextureFromImage(main_title);
    Image background = LoadImage("assets/background.png");
    ImageResize(&background, SCREEN_WIDTH, SCREEN_HEIGHT);
    textures.background = LoadTextureFromImage(background);
}
void UnloadTextures(void) {
    UnloadTexture(textures.branch);
    UnloadTexture(textures.red_leaf);
    UnloadTexture(textures.green_leaf);
    UnloadTexture(textures.mini_green_leaf);
    UnloadTexture(textures.mini_red_leaf);
    UnloadTexture(textures.arrow_down);
    UnloadTexture(textures.arrow_f_down);
    UnloadTexture(textures.arrow_up);
    UnloadTexture(textures.arrow_f_up);
    UnloadTexture(textures.arrow_left);
    UnloadTexture(textures.arrow_f_left);
    UnloadTexture(textures.arrow_right);
    UnloadTexture(textures.arrow_f_right);
    UnloadTexture(textures.main_title);
    UnloadTexture(textures.background);
}