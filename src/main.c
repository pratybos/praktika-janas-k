#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "genesis.h"

#include "defines.h"
#include "board.h"
#include "animation.h"
#include "score.h"
#include "game_move.h"
#include "cpu_move.h"
#include "game_settings.h"
#include "load_textures.h"
#include "html_gen.h"


#include <stdio.h>
#include <stddef.h>
#include "tinyfiledialogs.h"

struct Player {
    int color;
    int turns_remaining;
    int moves_remaining;
    enum CpuControlled {PLAYER, CPU_EASY, CPU_MEDIUM} cpu_controlled;
    int rating;
};

typedef enum GameScreen {
    SCREEN_MAIN_MENU = 0,
    SCREEN_OPTIONS,
    SCREEN_NEW_GAME,
    SCREEN_GAMEPLAY,
    SCREEN_ACHIEVEMENTS
} GameScreen;

struct Notation_Move {
    char letter;
    int number;
};

struct Board_Bounds bounds;
struct Tile_Amount tile_amount;
struct Turn_Move final_move;
struct Moves* head;
struct Animation animation[MAX][MAX];
struct Games textures;

struct Review_List {
    int turn_letter;
    int turn_number;
    int turn_board[MAX][MAX];
    struct Review_List* next;
    struct Review_List* prev;
};

struct Achievements {
    bool beat_easy_cpu;
    bool beat_hard_cpu;
    bool finish_first_game;
    bool watch_review;
};

struct Review_List review_list;
struct Review_List* review_head;
struct Review_List* review_end;
struct Board_Bounds review_bounds;


struct Review_List* Dvikrypt_Ideti_i_Gala(struct Review_List* review_head, int board[MAX][MAX], int letr, int num) {
    struct Review_List* newNode = (struct Review_List*)malloc(sizeof(struct Review_List));

    memcpy(newNode->turn_board, board, sizeof(board[0][0]) * MAX * MAX); 
    newNode->turn_letter = letr;
    newNode->turn_number = num;
    newNode->prev = NULL;
    newNode->next = NULL;

    if (review_head == NULL) {
        review_head = newNode;
        review_end = newNode;
        return review_head;
    }

    struct Review_List* temp = review_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
    review_end = newNode;

    return review_head;
}

struct Review_List* Destroy_Review_List(struct Review_List* head) {
    struct Review_List *current = head;
    struct Review_List *temp;

    while (current != NULL) {
        temp = current->next;
        MemFree(current);
        current = temp;
    }
    return NULL;
}

struct Turn_Move get_move_from_notation(struct Notation_Move notation_move, int color) {
    struct Turn_Move review_move;

    switch (notation_move.letter) {
            case 'N': 
                review_move.row = bounds.row_min - 1;
                review_move.col = bounds.col_min - 1 + notation_move.number;
                review_move.direction = ARROW_DOWN;
                review_move.color = color;
                break;
            case 'S': 
                review_move.row = bounds.row_max + 1;
                review_move.col = bounds.col_min - 1 + notation_move.number;
                review_move.direction = ARROW_UP;
                review_move.color = color;
                break;
            case 'W': 
                review_move.row = bounds.row_min - 1 + notation_move.number;
                review_move.col = bounds.col_min - 1;
                review_move.direction = ARROW_RIGHT;
                review_move.color = color;
                break;
            case 'E': 
                review_move.row = bounds.row_min - 1 + notation_move.number;
                review_move.col = bounds.col_max + 1;
                review_move.direction = ARROW_LEFT;
                review_move.color = color;
                break;
        }
    return review_move;
}

void read_file(struct Notation_Move all_moves[40], char* selectedFilePath) {

    FILE *fptr = fopen(selectedFilePath, "r");

    int review_board[MAX][MAX];
    bounds = InitBoard(review_board, VERTICAL);
    int i = 0; 
    char line[30];

    fgets(line, sizeof(line), fptr);
    while (1) {
        fgets(line, sizeof(line), fptr);

        if (line[0] == '\n')
            break;

        all_moves[i].letter = line[0];
        all_moves[i].number = line[1] - '0';   // converts char to int
        i++;

        if (line[2] == '-') {
            if (line[0] == 'N')
                all_moves[i].letter = 'E';
            else if (line[0] == 'E')
                all_moves[i].letter = 'S';
            else if (line[0] == 'S')
                all_moves[i].letter = 'W';
            else if (line[0] == 'W')
                all_moves[i].letter = 'N';
            all_moves[i].number = line[3] - '0'; 
            i++;
        }
    }

    fclose(fptr);
}

void write_file(char* filename, struct Notation_Move all_moves[40], int amount_of_moves, struct Score score) {
    FILE *fptr_w = fopen(filename, "w");

    for (int i = 0; i < amount_of_moves; i++) {
        if (all_moves[i].letter != '-')
            fprintf(fptr_w, "\n");
        fprintf(fptr_w, "%c%d", all_moves[i].letter, all_moves[i].number);
    }
    fprintf(fptr_w, "\n\n%d-%d", score.green, score.red);

    fclose(fptr_w);
}

struct Notation_Move ConvertToNotation(struct Turn_Move move, bool first_move) {
    struct Notation_Move notation_move;

    if (final_move.direction == ARROW_LEFT || final_move.direction == ARROW_RIGHT) {
        notation_move.number = final_move.row - bounds.row_min + 1;

        if (first_move) {

            if (final_move.direction == ARROW_LEFT)
                notation_move.letter = 'E';
            else
                notation_move.letter = 'W';
        }
        else
            notation_move.letter = '-';
    }
    else if (final_move.direction == ARROW_UP || final_move.direction == ARROW_DOWN) {
        notation_move.number = final_move.col - bounds.col_min + 1;

        if (first_move) {
            if (final_move.direction == ARROW_UP) 
                notation_move.letter = 'S';
            else
                notation_move.letter = 'N';
        }
        else
            notation_move.letter = '-';
    }

    return notation_move;
}

char* FileDialog(void) {
    // Define allowed file filters
    const char *filterPatterns[] = { "*.txt" };

    char *selectedFilePath = NULL;
    
    selectedFilePath = tinyfd_openFileDialog(
        "Select an File",      // Title
        "",                          // Default path
        1,                           // Number of filter patterns
        filterPatterns,              // Filter patterns array
        "Text Files (*.txt )", // Filter description
        0                            // Allow multiple selects (0 = No)
    );

    return selectedFilePath;
}

Sound placement_sound;
Sound push_sound_1;
Sound push_sound_2;
int push_sound = 1;
bool mute_audio = false;
bool disable_animations = false;

int main(void) {
    
    int board[MAX][MAX];      
    TileState tile_state;
    
    tile_amount.leaves = 10;
    tile_amount.branches = 5;

    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Leaves");

    InitAudioDevice();
    
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    
    SetTargetFPS(60);    
    
    placement_sound = LoadSound("assets/put.mp3");
    push_sound_1 = LoadSound("assets/push1.mp3");
    push_sound_2 = LoadSound("assets/push2.mp3");

    LoadTextures();
    GuiLoadStyleGenesis();

    Font myFont = LoadFontEx("assets/Kanit-SemiBold.ttf", 42, 0, 250);
    GuiSetFont(myFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 42);

    Vector2 mouse = { 0, 0 };

    int selected_col = 0, selected_row = 0;
    bool game_finished = true;
    bool color_changed = false;
    int player_num = 0;
    bool move_made = false;

    const float push_speed = 8.0f;  
    float delay_timer = 0.0f;
    
    struct Score score;

    struct Player player[2] = {
        {GREEN_LEAF, tile_amount.leaves, 1, PLAYER, 1000}, 
        {RED_LEAF, tile_amount.leaves, 0, CPU_EASY, 1000}
    };

    populate_elo_list();
    sort_elo_list();

    Start_Positions start_position = VERTICAL;

    struct Notation_Move notation_move;
    struct Notation_Move all_moves_per_match[40];
    int move_num = 0;
    bool first_move_per_turn = true;
    
    char* selected_file = NULL;
    struct Notation_Move notation_from_file[40];
    bool review_mode = false;
    struct Review_List* current_review_node;

    float notification_time = 0;
    char notification_text[50];
    float notification_PosX = 650;

    struct Achievements achievements = {false, false, false, false};

    GameScreen currentScreen = SCREEN_MAIN_MENU;

    bool block_mouse_dragging;
    bool mouse_on_sidebar;

    bool Spinner000EditMode = false;
    bool Spinner001EditMode = false;

    int ListView000ScrollIndex = 0;
    int ListView000Active = 0;
    int ListView001ScrollIndex = 0;
    int ListView001Active = player[0].cpu_controlled;
    int ListView002ScrollIndex = 0;
    int ListView002Active = player[1].cpu_controlled;

    Vector2 mini_green_pos = { 1130, 250 };
    Vector2 mini_red_pos = { 1130, 420 };

    while (!WindowShouldClose()) {

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            block_mouse_dragging = false;
        }

        // Zoom and mouse movement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !block_mouse_dragging && !mouse_on_sidebar) {
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

            switch (currentScreen)
            {
                case SCREEN_MAIN_MENU:
                {
                    // DrawText("MAIN MENU", 20, 20, 20, WHITE);
                    DrawTexture(textures.background, 0, 0, WHITE);
                    DrawTexture(textures.main_title, SCREEN_WIDTH / 2 - 261, 30, WHITE);

                    if (!game_finished) {
                        if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 290, 300, 50 }, "TESTI")) {                           
                            currentScreen = SCREEN_GAMEPLAY; 
                        }
                    }
                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 360, 300, 50 }, "NAUJAS ZAIDIMAS")) {                             
                        currentScreen = SCREEN_NEW_GAME;
                    }
                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 430, 300, 50 }, "NUSTATYMAI")) {
                        currentScreen = SCREEN_OPTIONS;
                    }
                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 500, 300, 50 }, "ANALIZE")) {
                        bounds = InitBoard(board, start_position);
                        player[0].moves_remaining = 1;
                        player[1].moves_remaining = 0;
                        player[0].turns_remaining = tile_amount.leaves;
                        player[1].turns_remaining = tile_amount.leaves;
                        game_finished = false;
                        color_changed = false;
                        score.green = 0;
                        score.red = 0;
                        player_num = 0;
                        move_num = 0;
                        first_move_per_turn = true;
                        tile_state = STATE_IDLE;

                        selected_file = FileDialog();
                        if (selected_file != NULL) {
                            read_file(notation_from_file, selected_file);
                            review_mode = true;
                            currentScreen = SCREEN_GAMEPLAY; 
                        }
                        break; 
                    }

                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 570, 300, 50 }, "TOP 100 HTML")) {
                        change_player_elo(player[0].rating, player[1].rating);
                        sort_elo_list();
                        build_html();
                        notification_time = 2000.0f;
                        strcpy(notification_text, "Failas sukurtas");
                    }

                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 640, 300, 50 }, "PASIEKIMAI")) {
                        currentScreen = SCREEN_ACHIEVEMENTS;
                    }

                } break;
                
                case SCREEN_ACHIEVEMENTS:
                {
                    DrawTexture(textures.background, 0, 0, GRAY);

                    if (achievements.watch_review)                        
                        DrawTextEx( myFont, TextFormat("PERZIURETAS ZAIDIMO IRASAS"), (Vector2) { 300, 200 }, 50, 1.0f, GOLD);
                    else                        
                        DrawTextEx( myFont, TextFormat("PERZIURETAS ZAIDIMO IRASAS"), (Vector2) { 300, 200 }, 50, 1.0f, GRAY);

               


                    if (achievements.finish_first_game)
                        DrawTextEx( myFont, TextFormat("PIRMAS ZAIDIMAS"), (Vector2) { 300, 300 }, 50, 1.0f, GOLD);
                    else
                        DrawTextEx( myFont, TextFormat("PIRMAS ZAIDIMAS"), (Vector2) { 300, 300 }, 50, 1.0f, GRAY);


                    if (achievements.beat_easy_cpu)
                        DrawTextEx( myFont, TextFormat("LAIMEJIMAS PRIES LENGVA KOMPIUTERI"), (Vector2) { 300, 400 }, 50, 1.0f, GOLD);
                    else
                        DrawTextEx( myFont, TextFormat("LAIMEJIMAS PRIES LENGVA KOMPIUTERI"), (Vector2) { 300, 400 }, 50, 1.0f, GRAY);

                    if (achievements.beat_hard_cpu)
                        DrawTextEx( myFont, TextFormat("LAIMEJIMAS PRIES SUNKU KOMPIUTERI"), (Vector2) { 300, 500 }, 50, 1.0f, GOLD);
                    else
                        DrawTextEx( myFont, TextFormat("LAIMEJIMAS PRIES SUNKU KOMPIUTERI"), (Vector2) { 300, 500 }, 50, 1.0f, GRAY);


                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 600, 300, 40 }, "GRIZTI I MENIU")) {
                        currentScreen = SCREEN_MAIN_MENU;
                    }
                }   break;

                case SCREEN_NEW_GAME:
                {
                    DrawTexture(textures.background, 0, 0, GRAY);

                    GuiLabel((Rectangle){ 370, 200, 400, 125 }, "ZALIEJI");
                    GuiListView((Rectangle){ 220, 300, 400, 125 }, "ZAIDEJAS;KOMPIUTERIS: LENGVAS;KOMPIUTERIS: SUNKUS", &ListView001ScrollIndex, &ListView001Active);
                    player[0].cpu_controlled = ListView001Active;

                    GuiLabel((Rectangle){ 800, 200, 400, 125 }, "RAUDONIEJI");
                    GuiListView((Rectangle){ 680, 300, 400, 125 }, "ZAIDEJAS;KOMPIUTERIS: LENGVAS;KOMPIUTERIS: SUNKUS", &ListView002ScrollIndex, &ListView002Active);
                    player[1].cpu_controlled = ListView002Active;
                    
                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 500, 300, 50 }, "PRADETI ZAIDIMA")) {                             
                        bounds = InitBoard(board, start_position);
                        AddArrows(board, ARROWS_ALL);
                        player[0].moves_remaining = 1;
                        player[1].moves_remaining = 0;
                        player[0].turns_remaining = tile_amount.leaves;
                        player[1].turns_remaining = tile_amount.leaves;
                        game_finished = false;
                        color_changed = false;
                        score.green = 0;
                        score.red = 0;
                        player_num = 0;
                        move_num = 0;
                        first_move_per_turn = true;
                        tile_state = STATE_IDLE;
                        review_mode = false;
                        currentScreen = SCREEN_GAMEPLAY; 

                        for (int row = 0; row < MAX; row++)
                            for (int col = 0; col < MAX; col++) {
                                animation[row][col].push_direction.x = 0;
                                animation[row][col].push_direction.y = 0;
                                animation[row][col].push_offset = 0;
                                animation[row][col].tile_tint = WHITE;
                            } 
                    }

                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 600, 300, 40 }, "GRIZTI I MENIU"))
                        currentScreen = SCREEN_MAIN_MENU;
                    
                }   break;

                case SCREEN_OPTIONS:
                {
                    DrawTexture(textures.background, 0, 0, GRAY);

                    GuiCheckBox((Rectangle){ SCREEN_WIDTH / 2 - 200, 180, 20, 20 }, " ISJUNGTI ANIMACIJAS", &disable_animations);
                    GuiCheckBox((Rectangle){ SCREEN_WIDTH / 2 - 200, 230, 20, 20 }, " ISJUNGTI GARSA", &mute_audio);

                    GuiLabel((Rectangle){ SCREEN_WIDTH / 2 - 440, 220, 400, 125 }, "PRADINE PADETIS");
                    GuiListView((Rectangle){ SCREEN_WIDTH / 2 - 200, 270, 400, 155 }, "VERTIKALI LINIJA;HORIZONTALI LINIJA;PLIUSO FORMACIJA;U FORMACIJA; ATSITIKTINE", &ListView000ScrollIndex, &ListView000Active);
                    start_position = ListView000Active;
                    
                    if (GuiSpinner((Rectangle){ SCREEN_WIDTH / 2 - 200, 450, 400, 40 }, "SAKU KIEKIS  ", &tile_amount.branches, 1, 9, Spinner000EditMode)) 
                    Spinner000EditMode = !Spinner000EditMode;
                    
                    if (GuiSpinner((Rectangle){ SCREEN_WIDTH / 2 - 200, 520, 400, 40 }, "LAPU KIEKIS  ", &tile_amount.leaves, 2, 20, Spinner001EditMode)) 
                    Spinner001EditMode = !Spinner001EditMode;

                    if (GuiButton((Rectangle){ SCREEN_WIDTH / 2 - 150, 600, 300, 40 }, "GRIZTI I MENIU"))
                        currentScreen = SCREEN_MAIN_MENU;
                }   break;



                case SCREEN_GAMEPLAY:
                {
                    if (GetMousePosition().x > SCREEN_WIDTH - SIDEBAR_WIDTH)
                        mouse_on_sidebar = true;
                    else
                        mouse_on_sidebar = false;
                        
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

                            if (review_mode || disable_animations) {
                                anim_posX = posX;
                                anim_posY = posY;
                                delay_timer = 0;
                            }
                            Color tile_color = animation[row][col].tile_tint;
                            
                            switch (board[row][col]) {
                                case GREEN_LEAF:
                                    DrawTexture(textures.green_leaf, anim_posX, anim_posY, tile_color);
                                    break;
                                case RED_LEAF:
                                    DrawTexture(textures.red_leaf, anim_posX, anim_posY, tile_color);
                                    break;
                                case BRANCH:
                                    DrawTexture(textures.branch, anim_posX, anim_posY, tile_color);
                                    break;
                                case ARROW_UP:
                                    if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                        if (row == selected_row && col == selected_col)
                                            DrawTexture(textures.arrow_f_up, posX, posY, player[player_num].color == GREEN_LEAF ? LIME : RED);
                                        else
                                            DrawTexture(textures.arrow_up, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    }
                                    break;
                                case ARROW_DOWN:
                                    if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                        if (row == selected_row && col == selected_col)
                                            DrawTexture(textures.arrow_f_down, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                        else
                                            DrawTexture(textures.arrow_down, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    }
                                    break;    
                                case ARROW_LEFT:
                                    if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                        if (row == selected_row && col == selected_col)
                                            DrawTexture(textures.arrow_f_left, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                        else
                                            DrawTexture(textures.arrow_left, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    }
                                    break;  
                                case ARROW_RIGHT:
                                    if (tile_state == STATE_IDLE && !player[player_num].cpu_controlled) {
                                        if (row == selected_row && col == selected_col)
                                            DrawTexture(textures.arrow_f_right, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                        else
                                            DrawTexture(textures.arrow_right, posX, posY, player[player_num].color == GREEN_LEAF ? GREEN : RED);
                                    }
                                    break;                  
                            }
                            posX += TILE_SIZE;
                        }
                        posY += TILE_SIZE;
                        posX = bounds.grid_origin.x;
                    }
                    EndMode2D();
                    
                    DrawRectangle(SCREEN_WIDTH - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, (Color) {48, 46, 43, 255});
                    
                    if (player[player_num].color == GREEN_LEAF)
                        DrawRectangleLinesEx((Rectangle){ 950, 160, 270, 180 }, 4.0f, SKYBLUE); 
                    if (player[player_num].color == RED_LEAF)
                        DrawRectangleLinesEx((Rectangle){ 950, 340, 270, 180 }, 4.0f, SKYBLUE); 

                    GuiLabel((Rectangle){ 1010, 150, 400, 125 }, "ZAIDEJAS 1");
                    for (int i = 1; i <= player[0].turns_remaining; i++) 
                        DrawTexture(textures.mini_green_leaf, mini_green_pos.x - 25 * (i / 2), mini_green_pos.y - (i % 2 * 10), WHITE);

                    GuiLabel((Rectangle){ 1010, 330, 400, 125 }, "ZAIDEJAS 2");
                    for (int i = 1; i <= player[1].turns_remaining; i++) 
                        DrawTexture(textures.mini_red_leaf, mini_red_pos.x - 25 * ((i + 1) / 2), mini_red_pos.y + (i % 2 * 10), WHITE);

                    
                    if (!review_mode && player[player_num].cpu_controlled && tile_state == STATE_IDLE && !game_finished) {
                        final_move = CpuMoves(board, player[player_num].color, head, player[player_num].cpu_controlled);
                        GameMove(board, final_move, PERMANENT);

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

                    if (review_mode == true && tile_state == STATE_IDLE && !game_finished) {
                        final_move = get_move_from_notation(notation_from_file[move_num], player[player_num].color);
                        GameMove(board, final_move, PERMANENT);
                        review_head = Dvikrypt_Ideti_i_Gala(review_head, board, notation_from_file[move_num].letter, notation_from_file[move_num].number);
                        move_made = true;
                    }


                    if (!review_mode && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && tile_state == STATE_IDLE && !game_finished) {
                        
                        // Selected from mouse coordinates
                        int selected_square = board[selected_row][selected_col];

                        final_move.col = selected_col;
                        final_move.row = selected_row;
                        final_move.color = player[player_num].color;

                        switch (selected_square) {
                            case ARROW_LEFT:
                                final_move.direction = ARROW_LEFT;
                                GameMove(board, final_move, PERMANENT);
                                AddArrows(board, ARROW_UP);
                                break;                            
                            case ARROW_RIGHT:
                                final_move.direction = ARROW_RIGHT;
                                GameMove(board, final_move, PERMANENT);
                                AddArrows(board, ARROW_DOWN);
                                break;                        
                            case ARROW_UP:
                                final_move.direction = ARROW_UP;
                                GameMove(board, final_move, PERMANENT);
                                AddArrows(board, ARROW_RIGHT);
                                break;                        
                            case ARROW_DOWN:
                                final_move.direction = ARROW_DOWN;
                                GameMove(board, final_move, PERMANENT);
                                AddArrows(board, ARROW_LEFT);
                                break;
                        }

                        if (selected_square == ARROW_LEFT || selected_square == ARROW_RIGHT || 
                            selected_square == ARROW_UP || selected_square == ARROW_DOWN) {
                                move_made = true;
                                block_mouse_dragging = true;
                        }
                    }
                    
                    if (move_made) {
                        notation_move = ConvertToNotation(final_move, first_move_per_turn);
                        all_moves_per_match[move_num] = notation_move;
                        move_num++;
                        if (first_move_per_turn)
                            first_move_per_turn = false;

                        player[player_num].moves_remaining--;
                        player[player_num].turns_remaining--;
                        score = CheckScore(board, CHECK_SCORE);

                        if (player[player_num].moves_remaining == 0) {
                            player_num = (player_num + 1) % (sizeof(player) / sizeof(player[0]));
                            player[player_num].moves_remaining = 2;
                            if (!review_mode)
                                AddArrows(board, ARROWS_ALL);
                            first_move_per_turn = true;
                        }
    
                        if (player[0].turns_remaining == 0 && player[1].turns_remaining == 0) {
                            game_finished = true;
                            AddArrows(board, ARROWS_NONE);
                            if (review_mode)
                                current_review_node = review_end;
                        }

                        move_made = false;
                        if (!review_mode && !mute_audio)
                            PlaySound(placement_sound);
                        tile_state = STATE_MADE_MOVE;
                        delay_timer = ANIM_DELAY_DURATION; 
                    }
                    
                    if (tile_state == STATE_MADE_MOVE) {
                        delay_timer -= GetFrameTime();
                        if (delay_timer <= 0.0f) {
                            tile_state = STATE_ANIMATING;
                            if (!review_mode && !mute_audio && !disable_animations)
                                switch (push_sound) {
                                    case 1: 
                                        PlaySound(push_sound_1); 
                                        push_sound = 2;
                                        break;
                                    case 2: 
                                        PlaySound(push_sound_2); 
                                        push_sound = 1;
                                        break;
                                }
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

                        if (review_mode && achievements.watch_review == false) {
                            achievements.watch_review = true;
                            notification_time = 2000.0f;
                            notification_PosX += 100.0f;
                            strcpy(notification_text, "Pasiekimas atrakintas");
                        }

                        if (tile_state == STATE_POST_ANIMATION && !color_changed && !review_mode) {
                            CheckScore(board, DIM_UNSCORING_TILES);
                            color_changed = true;
                            float expected_rating_1;
                            float expected_rating_2;
                            expected_rating_1 = 1 / (1 + pow(10, (player[1].rating - player[0].rating) / 400.0f));
                            expected_rating_2 = 1 / (1 + pow(10, (player[0].rating - player[1].rating) / 400.0f));
                            if (score.green > score.red) {
                                player[0].rating = player[0].rating + 30 * (1 - expected_rating_1);
                                player[1].rating = player[1].rating + 30 * (0 - expected_rating_2);
                            }
                            else if (score.green < score.red) {
                                player[0].rating = player[0].rating + 30 * (0 - expected_rating_1);
                                player[1].rating = player[1].rating + 30 * (1 - expected_rating_2);
                            }
                            else {
                                player[0].rating = player[0].rating + 30 * (0.5 - expected_rating_1);
                                player[1].rating = player[0].rating + 30 * (0.5 - expected_rating_2);
                            }

                            if (achievements.finish_first_game == false) {
                                achievements.finish_first_game = true;
                                notification_time = 2000.0f;
                                notification_PosX += 100.0f;
                                strcpy(notification_text, "Pasiekimas atrakintas");
                            }

                            if (player[0].cpu_controlled == PLAYER && 
                                player[1].cpu_controlled == CPU_EASY &&
                                achievements.beat_easy_cpu == false)

                                if (score.green > score.red) {
                                    achievements.beat_easy_cpu = true;
                                    notification_time = 2000.0f;
                                    notification_PosX += 100.0f;
                                    strcpy(notification_text, "Pasiekimas atrakintas");
                                }
                            if (player[1].cpu_controlled == PLAYER && 
                                player[0].cpu_controlled == CPU_EASY && 
                                achievements.beat_easy_cpu == false)
                                
                                if (score.green < score.red) {
                                    achievements.beat_easy_cpu = true;
                                    notification_time = 2000.0f;
                                    notification_PosX += 100.0f;
                                    strcpy(notification_text, "Pasiekimas atrakintas");
                                }
                            if (player[0].cpu_controlled == PLAYER && 
                                player[1].cpu_controlled == CPU_MEDIUM && 
                                achievements.beat_hard_cpu == false)

                                if (score.green > score.red) {
                                    achievements.beat_hard_cpu = true;
                                    notification_time = 2000.0f;
                                    notification_PosX += 100.0f;
                                    strcpy(notification_text, "Pasiekimas atrakintas");
                                }
                            if (player[1].cpu_controlled == PLAYER && 
                                player[0].cpu_controlled == CPU_MEDIUM && 
                                achievements.beat_hard_cpu == false)

                                if (score.green < score.red) {
                                    achievements.beat_hard_cpu = true;
                                    notification_time = 2000.0f;
                                    notification_PosX += 100.0f;
                                    strcpy(notification_text, "Pasiekimas atrakintas");
                                }
                        }

                        if (score.green > score.red)
                            DrawTextEx( myFont, TextFormat("ZALIAS LAIMI"), (Vector2) { 980, 50 }, 50, 1.0f, LIME);
                        else if (score.green < score.red)
                            DrawTextEx( myFont, TextFormat("RAUDONAS LAIMI"), (Vector2) { 950, 50 }, 50, 1.0f, MAROON);
                        else
                            DrawTextEx( myFont, TextFormat("LYGIOSIOS"), (Vector2) { 980, 50 }, 50, 1.0f, YELLOW);

                        DrawTextEx( myFont, TextFormat("   %d", score.green), (Vector2) { 1030, 80 }, 50, 1.0f, LIME);
                        DrawTextEx( myFont, TextFormat("      -"), (Vector2) { 1030, 80 }, 50, 1.0f, LIGHTGRAY);
                        DrawTextEx( myFont, TextFormat("         %d", score.red), (Vector2) { 1030, 80 }, 50, 1.0f, MAROON);
                    }

                    if (review_mode && game_finished) {
                        if (GuiButton((Rectangle){ 960, 500, 60, 60 }, "<<")) {
                            memcpy(board, review_head->turn_board, sizeof(review_head->turn_board[0][0]) * MAX * MAX);
                            current_review_node = review_head;
                        }
                        if (GuiButton((Rectangle){ 1025, 500, 60, 60 }, "<")) {
                            if (current_review_node->prev != NULL) {
                                current_review_node = current_review_node->prev;
                                memcpy(board, current_review_node->turn_board, sizeof(current_review_node->turn_board[0][0]) * MAX * MAX);
                            }
                        }
                        if (GuiButton((Rectangle){ 1090, 500, 60, 60 }, ">")) {
                            if (current_review_node->next != NULL) {
                                current_review_node = current_review_node->next;
                                memcpy(board, current_review_node->turn_board, sizeof(current_review_node->turn_board[0][0]) * MAX * MAX);
                            }
                        }
                        if (GuiButton((Rectangle){ 1155, 500, 60, 60 }, ">>")) {
                            memcpy(board, review_end->turn_board, sizeof(review_head->turn_board[0][0]) * MAX * MAX);
                            current_review_node = review_end;
                        }
                    }

                    if (game_finished && !review_mode && 
                        start_position == VERTICAL && 
                        tile_amount.leaves == 10 && 
                        tile_amount.branches == 5)

                        if (GuiButton((Rectangle){ 940, 570, 300, 50 }, "ISSAUGOTI I FAILA")) {
                            write_file("notation.txt", all_moves_per_match, tile_amount.leaves * 2, score);
                            notification_time = 2000.0f;
                            strcpy(notification_text, "Failas sukurtas");
                        }

                    if (GuiButton((Rectangle){ 940, 640, 300, 50 }, "GRIZTI I MENIU")) {
                        review_head = Destroy_Review_List(review_head);
                        review_mode = false;
                        currentScreen = SCREEN_MAIN_MENU;
                    }
                } break;

                default: break;
            }

            if (notification_time > 0) {
                if (notification_PosX > 650)
                    notification_PosX = notification_PosX - 0.001f * notification_time;
                else
                    notification_PosX = 650;

                DrawTextEx(
                    myFont,
                    TextFormat("%s", notification_text),
                    (Vector2) { 10, notification_PosX },
                    50,
                    1.0f,
                    WHITE
                );
                notification_time -= 8;
            }
            

        EndDrawing();
    }
    UnloadSound(placement_sound);
    UnloadSound(push_sound_1);
    UnloadSound(push_sound_2);
    UnloadFont(myFont);
    UnloadTextures();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
