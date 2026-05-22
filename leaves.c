#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TILE_SIZE 80
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BRANCH_AMOUNT 5
#define TILES_PER_PLAYER 10
#define TURNS_PER_PLAYER 10
#define EMPTY_SPACE 0
#define GREEN_LEAF 1
#define RED_LEAF 2
#define BRANCH 3

#define ARROW_LEFT 11
#define ARROW_RIGHT 12
#define ARROW_UP 13
#define ARROW_DOWN 14
#define ARROWS_ALL 15
#define ARROWS_NONE 16

#define MAX 90
#define CENTER (MAX / 2)

int col_min, col_max, row_min, row_max;
Vector2 grid_origin;

struct Player {
    int color;
    int turns_remaining;
    int moves_remaining;
    bool clockwise_turn;
    bool CPU_controlled;
};

void InitBoard(int arr[MAX][MAX], int type) {

    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            arr[row][col] = 0;
        }
    }
    row_min = row_max = col_min = col_max = CENTER;

    switch (type) {
        case 1:
            // Vertical line start
            row_min = CENTER - BRANCH_AMOUNT / 2;
            row_max = CENTER + BRANCH_AMOUNT / 2;
            col_min = col_max = CENTER;  

            for (int i = row_min; i <= row_max; i++) {
                arr[i][CENTER] = BRANCH;
            }
            break;

        case 2:
            // Horizontal start
            col_min = CENTER - BRANCH_AMOUNT / 2;
            col_max = CENTER + BRANCH_AMOUNT / 2;
            row_min = row_max = CENTER;  

            for (int i = col_min; i <= col_max; i++) {
                arr[CENTER][i] = BRANCH;
            }
            break;

        case 3:
            // Plus start
            arr[CENTER][CENTER] = 3;    // Center
            for (int i = 1; i < BRANCH_AMOUNT; i++) {
                switch (i % 4) {
                    case 1:         // Left
                        col_min--;
                        arr[CENTER][col_min] = BRANCH;
                        break;
                    case 2:         // UP
                        row_min--;
                        arr[row_min][CENTER] = BRANCH;
                        break;
                    case 3:         // RIGHT
                        col_max++;
                        arr[CENTER][col_max] = BRANCH;
                        break;
                    case 0:         // DOWN
                        row_max++;
                        arr[row_max][CENTER] = BRANCH;
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
                        col_min--;
                        arr[row_min][col_min] = BRANCH;
                        break;
                    case 2:         // Right
                        col_max++;
                        arr[row_min][col_max] = BRANCH;
                        break;
                    case 3:         // LEFT-UP
                        row_min--;
                        arr[row_min][col_min] = BRANCH;
                        break;
                    case 0:         // RIGHT-UP
                        arr[row_min][col_max] = BRANCH;
                        break;
                }
            }
            break;
    }

    int grid_width = (col_max - col_min + 3) * TILE_SIZE;
    int grid_height = (row_max - row_min + 3) * TILE_SIZE; 
    grid_origin = (Vector2) {
        (SCREEN_WIDTH - grid_width) / 2,
        (SCREEN_HEIGHT - grid_height) / 2
    };
}

void AddArrows(int arr[MAX][MAX], int mode) {

        // Clear arrows
        for (int row = row_min - 1; row <= row_max + 1; row++)
            for (int col = col_min - 1; col <= col_max + 1; col++) 
                if (arr[row][col] == ARROW_LEFT || 
                    arr[row][col] == ARROW_RIGHT || 
                    arr[row][col] == ARROW_UP || 
                    arr[row][col] == ARROW_DOWN )
                        arr[row][col] = 0;
        
        if (mode == ARROWS_NONE)
            return;

        if (mode == ARROW_DOWN || mode == ARROWS_ALL)
            for (int i = col_min; i <= col_max; i++)
                arr[row_min - 1][i] = ARROW_DOWN;
        
        if (mode == ARROW_UP || mode == ARROWS_ALL)
            for (int i = col_min; i <= col_max; i++)
                arr[row_max + 1][i] = ARROW_UP;

        if (mode == ARROW_RIGHT || mode == ARROWS_ALL)
            for (int i = row_min; i <= row_max; i++)
                arr[i][col_min - 1] = ARROW_RIGHT;

        if (mode == ARROW_LEFT || mode == ARROWS_ALL)
            for (int i = row_min; i <= row_max; i++)
                arr[i][col_max + 1] = ARROW_LEFT;
        
}

void GameMove(int arr[MAX][MAX], int turn_color, int direction, int col, int row, bool permanent) {
    // Tile pushing based on direction
    int i, j;
    switch (direction) {

        case ARROW_LEFT:
            i = col_max;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i--;

            j = i - 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j--;

            if (j < col_min && permanent == true) {
                col_min--;
                grid_origin.x -= TILE_SIZE;
            }

            for (; j <= i; j++)
                arr[row][j] = arr[row][j + 1];
            
            arr[row][i] = turn_color;
            break;

        case ARROW_RIGHT:
            i = col_min;
            while (arr[row][i] == EMPTY_SPACE || arr[row][i] > 3)
                i++;

            j = i + 1;
            while (arr[row][j] == RED_LEAF || arr[row][j] == GREEN_LEAF || arr[row][j] == BRANCH)
                j++;

            if (j > col_max && permanent == true) {
                col_max++;
                // grid_origin.x += TILE_SIZE;
            }

            for (; j >= i; j--)
                arr[row][j] = arr[row][j - 1];

            arr[row][i] = turn_color;
            break;
            
        case ARROW_UP:
            i = row_max;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i--;

            j = i - 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j--;

            if (j < row_min && permanent == true) {
                row_min--;
                grid_origin.y -= TILE_SIZE;
            }

            for (; j <= i; j++)
                arr[j][col]= arr[j + 1][col];
            
            arr[i][col] = turn_color;
            break;

        case ARROW_DOWN:
            i = row_min;
            while (arr[i][col] == EMPTY_SPACE || arr[i][col] > 3)
                i++;

            j = i + 1;
            while (arr[j][col] == RED_LEAF || arr[j][col] == GREEN_LEAF || arr[j][col]== BRANCH)
                j++;

            if (j > row_max && permanent == true) {
                row_max++;
                // grid_origin.y += TILE_SIZE;
            }

            for (; j >= i; j--)
                arr[j][col]= arr[j - 1][col];
            
            arr[i][col] = turn_color;
            break;
    }
}

struct Score {
    int green;
    int red;
};

struct Score CheckScore(int arr[MAX][MAX]) {

    struct Score score;
    score.green = 0;
    score.red = 0;
    
    for (int row = row_min; row <= row_max; row++) {
        for (int col = col_min; col <= col_max; col++) {
            if (arr[row + 1][col] == BRANCH || 
                arr[row - 1][col] == BRANCH || 
                arr[row][col + 1] == BRANCH || 
                arr[row][col - 1] == BRANCH) {
                    if (arr[row][col] == GREEN_LEAF)
                        score.green += 1;
                    else if (arr[row][col] == RED_LEAF)
                        score.red += 1;
                }
            }
        }

    return score;        
}


struct Moves {
    int row;
    int col;
    int direction;
    int score;
    struct Moves* next;
};
struct Moves* head;

struct Moves* InsertToBack(struct Moves* head, int direction, int row, int col) {
    struct Moves* temp = (struct Moves*)malloc(sizeof(struct Moves));
    temp->row = row;
    temp->col = col;
    temp->direction = direction;
    temp->score = 0;
    temp->next = NULL;
    if (head == NULL)
        head = temp;
    else {
        struct Moves* temp1 = head;
        while (temp1->next != NULL)
            temp1 = temp1->next;
        temp1->next = temp;
    }
    return head;
}

struct Moves* destroy(struct Moves* head) {
    struct Moves *current = head;
    struct Moves *temp;

    while (current != NULL) {
        temp = current->next;
        free(current);
        current = temp;
    }
    head = NULL;
    return head;
}

int CpuMoves(int board[MAX][MAX], int theory_board[MAX][MAX], int turn_color, struct Moves* head) { 
    

    /*
    1. Trinamas susietas ejimu sarasas (Move)
    2. Visi galimi ejimai surasomi i susieta sarasa
    3. Ciklas:
        Laikinas lentos masyvas kopijuojamas is pagrindinio masyvo board
        Ejimas is saraso atliekamas nukopijuotame masyve
        Skaiciuojamas tasku pokytis pries ir po ejimo
        Issaugomas didziausias tasku pokytis i max_score
    4. Is susieto saraso Moves trinami ejimai kuriu verte mazesne uz max_score
    5. Is likusiu ejimu sarase pasirenkamas atsitiktinis
    6. Pasirinktas ejimas pritaikomas pagrindiniam masyvui "board"
    */

    head = destroy(head);

    for (int row = row_min - 1; row <= row_max + 1; row++) {
        for (int col = col_min - 1; col <= col_max + 1; col++) {
            if (board[row][col] == ARROW_LEFT || 
                board[row][col] == ARROW_RIGHT || 
                board[row][col] == ARROW_UP || 
                board[row][col] == ARROW_DOWN ) {
                    head = InsertToBack(head, board[row][col], row, col);
            }
        }
    }



    struct Moves* current = head, *result, *prev;
    struct Score score_after;

    int max_score = 0;
    while (current != NULL) {
        memcpy(theory_board, board, sizeof(board[0][0]) * MAX * MAX); 
        GameMove(theory_board, turn_color, current->direction, current->col, current->row, false);
        score_after = CheckScore(theory_board);
        if (turn_color == RED_LEAF)
            current->score = score_after.red - score_after.green;
        else
            current->score = score_after.green - score_after.red;
            
        if (current->score > max_score)
            max_score = current->score;

        current = current->next;
    }

    current = head;
    prev = head;
    while (current != NULL) {
        if (current->score < max_score) {
            if (current == head) {
                head = current->next;
                free(current);
                current = head;
                prev = head;
            }
            else {
                prev->next = current->next;  // (n+1)th Node
                free(current);
                current = prev->next;
            }
        }
        else {
            prev = current;
            current = current->next;
        }
    }    
    
    
    srand(time(NULL));
    current = head;
    result = head;

    for (int n = 2; current != NULL; n++) {
        // change result with probability 1/n
        if (rand() % n == 0)
           result = current;

        current = current->next;
    }

    GameMove(board, turn_color, result->direction, result->col, result->row, true);

    return result->direction;
}

int main(void) {
    int board[MAX][MAX];    
    InitBoard(board, 1);
    AddArrows(board, ARROWS_ALL);

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

    Vector2 mouse = { 0, 0 };

    int selected_col = 0, selected_row = 0;
    bool game_finished = false;
    struct Score score;
    int cpu_direction = 0;

    int theory_board[MAX][MAX];

    struct Player player[2] = {
        {GREEN_LEAF, TURNS_PER_PLAYER, 1, false, false}, 
        {RED_LEAF, TURNS_PER_PLAYER, 0, false, true}
    };

    int player_num = 0;
    bool move_made = false;


    while (!WindowShouldClose()) {

        // RESET BOARD
        if (IsKeyPressed(KEY_ONE)) {
            InitBoard(board, 1);
        }
        if (IsKeyPressed(KEY_TWO)) {
            InitBoard(board, 2);
        }
        if (IsKeyPressed(KEY_THREE)) {
            InitBoard(board, 3);
        }
        if (IsKeyPressed(KEY_FOUR)) {
            InitBoard(board, 4);
        }

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || 
            IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_FOUR)) {

            AddArrows(board, ARROWS_ALL);
            player[0].moves_remaining = 1;
            player[1].moves_remaining = 0;
            player[0].turns_remaining = TURNS_PER_PLAYER;
            player[1].turns_remaining = TURNS_PER_PLAYER;
            game_finished = false;
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
                int posX = grid_origin.x;
                int posY = grid_origin.y;
                // Draw board tiles
                for (int row = row_min - 1; row <= row_max + 1; row++) {
                    for (int col = col_min - 1; col <= col_max + 1; col++) {
                        switch (board[row][col]) {
                            case GREEN_LEAF:
                                DrawTexture(green_LEAF_texture, posX, posY, WHITE);
                                break;
                            case RED_LEAF:
                                DrawTexture(red_LEAF_texture, posX, posY, WHITE);
                                break;
                            case BRANCH:
                                DrawTexture(branch_texture, posX, posY, WHITE);
                                break;
                            case ARROW_UP:
                                DrawTexture(arrow_up_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;
                            case ARROW_DOWN:
                                DrawTexture(arrow_down_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;    
                            case ARROW_LEFT:
                                DrawTexture(arrow_left_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;  
                            case ARROW_RIGHT:
                                DrawTexture(arrow_right_texture, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                break;                  
                        }
                        posX += TILE_SIZE;
                    }
                    posY += TILE_SIZE;
                    posX = grid_origin.x;
                }


                if (player[player_num].CPU_controlled && game_finished == false) {
                    cpu_direction = CpuMoves(board, theory_board, player[player_num].color, head);

                    switch (cpu_direction) {
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

                mouse = GetScreenToWorld2D(GetMousePosition(), camera);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game_finished) {
                    selected_col = (mouse.x - grid_origin.x) / TILE_SIZE + col_min - 1;
                    selected_row = (mouse.y - grid_origin.y) / TILE_SIZE + row_min - 1;  

                    int selected_square = board[selected_row][selected_col];

                    switch (selected_square) {
                        case ARROW_LEFT:
                            GameMove(board, player[player_num].color, ARROW_LEFT, selected_col, selected_row, true);
                            AddArrows(board, ARROW_UP);
                            break;                            
                        case ARROW_RIGHT:
                            GameMove(board, player[player_num].color, ARROW_RIGHT, selected_col, selected_row, true);
                            AddArrows(board, ARROW_DOWN);
                            break;                        
                        case ARROW_UP:
                            GameMove(board, player[player_num].color, ARROW_UP, selected_col, selected_row, true);
                            AddArrows(board, ARROW_RIGHT);
                            break;                        
                        case ARROW_DOWN:
                            GameMove(board, player[player_num].color, ARROW_DOWN, selected_col, selected_row, true);
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
                    score = CheckScore(board);

                    if (player[player_num].turns_remaining > 0 && player[player_num].moves_remaining == 0) {
                        player_num = (player_num + 1) % (sizeof(player) / sizeof(player[0]));
                        player[player_num].moves_remaining = 2;
                        AddArrows(board, ARROWS_ALL);
                    }
                    
                    if (player[player_num].turns_remaining == 0) {
                        if (player[player_num].color == RED_LEAF) {
                            player_num = 0;
                            player[player_num].moves_remaining = 1;
                            AddArrows(board, ARROWS_ALL);
                        }
                        else {
                            game_finished = true;
                            AddArrows(board, ARROWS_NONE);
                        }
                    }

                    move_made = false;
                }

            EndMode2D();

            if (game_finished) {

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
