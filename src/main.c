#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"

#include "defines.h"
#include "board.h"
#include "animation.h"
#include "score.h"
#include "game_move.h"
#include "cpu_move.h"

struct Player {
    int color;
    int turns_remaining;
    int moves_remaining;
    enum CpuControlled {PLAYER, CPU_EASY, CPU_MEDIUM} cpu_controlled;
};

struct Board_Bounds bounds;
struct Turn_Move final_move;
struct Moves* head;
struct Animation animation[MAX][MAX];

int main(void) {
    int board[MAX][MAX];      
    
    float push_speed = 8.0f;  
    TileState tile_state;
    float delay_timer = 0.0f;
    
    bounds = InitBoard(board, 1);
    AddArrows(board, ARROWS_ALL);
    tile_state = STATE_IDLE;

    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Leaves");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetTargetFPS(60);     

    Image branch_image = LoadImage("assets/branch.png");
    Image green_LEAF_image = LoadImage("assets/green.png");
    Image red_LEAF_image = LoadImage("assets/red.png");

    ImageResize(&branch_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&green_LEAF_image, TILE_SIZE, TILE_SIZE);
    ImageResize(&red_LEAF_image, TILE_SIZE, TILE_SIZE);

    Texture2D branch_texture = LoadTextureFromImage(branch_image);
    Texture2D green_LEAF_texture = LoadTextureFromImage(green_LEAF_image);
    Texture2D red_LEAF_texture = LoadTextureFromImage(red_LEAF_image);

    Image arrow_image = LoadImage("assets/arrow.png");
    ImageResize(&arrow_image, TILE_SIZE, TILE_SIZE);

    Texture2D arrow_up_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_right_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_down_texture = LoadTextureFromImage(arrow_image);
    ImageRotate(&arrow_image, 90);
    Texture2D arrow_left_texture = LoadTextureFromImage(arrow_image);

    Image arrow_f_image = LoadImage("assets/arrow_f.png");
    ImageResize(&arrow_f_image, TILE_SIZE, TILE_SIZE);

    Texture2D arrow_f_up_texture = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    Texture2D arrow_f_right_texture = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    Texture2D arrow_f_down_texture = LoadTextureFromImage(arrow_f_image);
    ImageRotate(&arrow_f_image, 90);
    Texture2D arrow_f_left_texture = LoadTextureFromImage(arrow_f_image);

    Vector2 mouse = { 0, 0 };

    int selected_col = 0, selected_row = 0;
    bool game_finished = false;
    bool color_changed = false;
    int theory_board[MAX][MAX];
    int player_num = 0;
    bool move_made = false;
    
    struct Score score;

    struct Player player[2] = {
        {GREEN_LEAF, TURNS_PER_PLAYER, 1, PLAYER}, 
        {RED_LEAF, TURNS_PER_PLAYER, 0, CPU_MEDIUM}
    };

    while (!WindowShouldClose()) {

        // RESET BOARD
        if (IsKeyPressed(KEY_ONE)) {
            bounds = InitBoard(board, 1);
        }
        if (IsKeyPressed(KEY_TWO)) {
            bounds = InitBoard(board, 2);
        }
        if (IsKeyPressed(KEY_THREE)) {
            bounds = InitBoard(board, 3);
        }
        if (IsKeyPressed(KEY_FOUR)) {
            bounds = InitBoard(board, 4);
        }

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || 
            IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_FOUR)) {

            AddArrows(board, ARROWS_ALL);
            player[0].moves_remaining = 1;
            player[1].moves_remaining = 0;
            player[0].turns_remaining = TURNS_PER_PLAYER;
            player[1].turns_remaining = TURNS_PER_PLAYER;
            game_finished = false;
            color_changed = false;
            score.green = 0;
            score.red = 0;
        }

        // Zoom and mouse movement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;
            float scaleFactor = 1.0f + (0.25f*fabsf(wheel));
            if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
            camera.zoom = Clamp(camera.zoom*scaleFactor, 0.3f, 3.0f);
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode2D(camera);
                mouse = GetScreenToWorld2D(GetMousePosition(), camera);
                selected_col = (mouse.x - bounds.grid_origin.x) / TILE_SIZE + bounds.col_min - 1;
                selected_row = (mouse.y - bounds.grid_origin.y) / TILE_SIZE + bounds.row_min - 1;  

                int posX = bounds.grid_origin.x;
                int posY = bounds.grid_origin.y;
                // Draw board tiles
                for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++) {
                    for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) {

                        int anim_posX = posX + animation[row][col].push_offset * animation[row][col].push_direction.x;
                        int anim_posY = posY + animation[row][col].push_offset * animation[row][col].push_direction.y;
                        Color tile_color = animation[row][col].tile_tint;
                        
                        switch (board[row][col]) {
                            case GREEN_LEAF:
                                DrawTexture(green_LEAF_texture, anim_posX, anim_posY, tile_color);
                                break;
                            case RED_LEAF:
                                DrawTexture(red_LEAF_texture, anim_posX, anim_posY, tile_color);
                                break;
                            case BRANCH:
                                DrawTexture(branch_texture, anim_posX, anim_posY, tile_color);
                                break;
                            case ARROW_UP:
                                if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                    if (row == selected_row && col == selected_col)
                                        DrawTexture(arrow_f_up_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    else
                                        DrawTexture(arrow_up_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                }
                                break;
                            case ARROW_DOWN:
                                if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                    if (row == selected_row && col == selected_col)
                                        DrawTexture(arrow_f_down_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    else
                                        DrawTexture(arrow_down_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                }
                                break;    
                            case ARROW_LEFT:
                                if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                    if (row == selected_row && col == selected_col)
                                        DrawTexture(arrow_f_left_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    else
                                        DrawTexture(arrow_left_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                }
                                break;  
                            case ARROW_RIGHT:
                                if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                    if (row == selected_row && col == selected_col)
                                        DrawTexture(arrow_f_right_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    else
                                        DrawTexture(arrow_right_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                }
                                break;                  
                        }
                        posX += TILE_SIZE;
                    }
                    posY += TILE_SIZE;
                    posX = bounds.grid_origin.x;
                }

                if (player[player_num].cpu_controlled && tile_state == STATE_IDLE && !game_finished) {
                    final_move = CpuMoves(board, theory_board, player[player_num].color, head, player[player_num].cpu_controlled);
                    GameMove(board, final_move, true);

                    switch (final_move.direction) {
                        case ARROW_LEFT:
                            AddArrows(board, ARROW_UP);
                            break;                            
                        case ARROW_RIGHT:
                            AddArrows(board, ARROW_DOWN);
                            break;                        
                        case ARROW_UP:
                            AddArrows(board, ARROW_RIGHT);
                            break;                        
                        case ARROW_DOWN:
                            AddArrows(board, ARROW_LEFT);
                            break;
                    }
                    move_made = true;              
                }


                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && tile_state == STATE_IDLE && !game_finished) {

                    // Selected from mouse coordinates
                    int selected_square = board[selected_row][selected_col];

                    final_move.col = selected_col;
                    final_move.row = selected_row;
                    final_move.color = player[player_num].color;

                    switch (selected_square) {
                        case ARROW_LEFT:
                            final_move.direction = ARROW_LEFT;
                            GameMove(board, final_move, true);
                            AddArrows(board, ARROW_UP);
                            break;                            
                        case ARROW_RIGHT:
                            final_move.direction = ARROW_RIGHT;
                            GameMove(board, final_move, true);
                            AddArrows(board, ARROW_DOWN);
                            break;                        
                        case ARROW_UP:
                            final_move.direction = ARROW_UP;
                            GameMove(board, final_move, true);
                            AddArrows(board, ARROW_RIGHT);
                            break;                        
                        case ARROW_DOWN:
                            final_move.direction = ARROW_DOWN;
                            GameMove(board, final_move, true);
                            AddArrows(board, ARROW_LEFT);
                            break;
                    }

                    if (selected_square == ARROW_LEFT || selected_square == ARROW_RIGHT || 
                        selected_square == ARROW_UP || selected_square == ARROW_DOWN) {
                            move_made = true;
                    }
                }
                
                if (move_made) {
                    player[player_num].moves_remaining--;
                    player[player_num].turns_remaining--;
                    score = CheckScore(board, false);

                    if (player[player_num].moves_remaining == 0) {
                        player_num = (player_num + 1) % (sizeof(player) / sizeof(player[0]));
                        player[player_num].moves_remaining = 2;
                        AddArrows(board, ARROWS_ALL);
                    }
   
                    if (player[0].turns_remaining == 0 && player[1].turns_remaining == 0) {
                        game_finished = true;
                        AddArrows(board, ARROWS_NONE);
                    }

                    move_made = false;
                    tile_state = STATE_MADE_MOVE;
                    delay_timer = DELAY_DURATION; 
                }

            EndMode2D();

            
            if (tile_state == STATE_MADE_MOVE) {
                delay_timer -= GetFrameTime();
                if (delay_timer <= 0.0f) {
                    tile_state = STATE_ANIMATING;
                }
            }

            if (tile_state == STATE_POST_ANIMATION) {
                delay_timer -= GetFrameTime();
                if (delay_timer <= 0.0f) {
                    tile_state = STATE_IDLE;
                }
            }
            

            if (tile_state == STATE_ANIMATING) {
                for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++) {
                    for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) {
                        if (animation[row][col].push_offset > 0) {
                            animation[row][col].push_offset -= push_speed;
                            if (animation[row][col].push_offset <= 0) {
                                animation[row][col].push_offset = 0;
                                animation[row][col].push_direction.x = 0;
                                animation[row][col].push_direction.y = 0;
                                tile_state = STATE_POST_ANIMATION;
                            }
                        }
                    }
                }
            }

            if (game_finished) {
                if (tile_state == STATE_POST_ANIMATION && !color_changed) {
                    CheckScore(board, true);
                    color_changed = true;
                }

                if (score.green > score.red)
                    DrawText(TextFormat("ZALIAS LAIMI"), 20, 20, 40, GREEN);
                else if (score.green < score.red)
                    DrawText(TextFormat("RAUDONAS LAIMI"), 20, 20, 40, RED);
                else
                    DrawText(TextFormat("LYGIOSIOS"), 20, 20, 40, YELLOW);

                DrawText(TextFormat("   %d", score.green), 20, 60, 50, GREEN);
                DrawText(TextFormat("     -"), 20, 60, 50, WHITE);
                DrawText(TextFormat("       %d", score.red), 20, 60, 50, RED);
            }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
