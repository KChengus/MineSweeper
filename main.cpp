// Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <vector>
#include <time.h>
#include "Cell.cpp"


#define BOMBS 40


// Screen dimension constants
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 900;

// Grid constants
const int CELL_WIDTH = 60;
const int CELL_HEIGHT = 60;
const int GRID_COLS = WINDOW_WIDTH / CELL_WIDTH;
const int GRID_ROWS = WINDOW_HEIGHT / CELL_HEIGHT;
const int GRID_SIZE = GRID_COLS * GRID_ROWS;
const int direc[8][2] = {
    {-1,1},
    {0,1},
    {1,1},
    {1,0},
    {1,-1},
    {0,-1},
    {-1,-1},
    {-1,0}
};
std::vector<Cell> cells;

// The window we'll be rendering to 
SDL_Window* window = NULL;

// the image we will load and show on the screen
SDL_Surface* imageSurface = NULL;
SDL_Rect srcRect;
// Renderer
SDL_Renderer* renderer = NULL;

// forward decleration
bool init();

bool loadMedia();

void create_board();
void bfs(int c, int r, int visited[GRID_SIZE]);
void close();

int main( int argc, char* args[] ) {
    // Consts
    SDL_Color bg_color = {100, 100, 100, SDL_ALPHA_OPAQUE};
    // rand num
    srand(time(NULL));

    
    if (!init()) {
        printf("Failed to initialize!\n");
        return EXIT_FAILURE;
    } 
    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return EXIT_FAILURE;
    }

    // GUI const 
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_Rect one{CELL_WIDTH*0, 0, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect two{CELL_WIDTH*1, 0, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect three{CELL_WIDTH*2, 0, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect four{CELL_WIDTH*3, 0, CELL_WIDTH, CELL_HEIGHT};

    SDL_Rect bomb{CELL_WIDTH*0, 60, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect flag{CELL_WIDTH*1, 60, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect cellImg{CELL_WIDTH*2, 60, CELL_WIDTH, CELL_HEIGHT};
    SDL_Rect button{CELL_WIDTH*3, 60, CELL_WIDTH, CELL_HEIGHT};

    std::vector<SDL_Rect> images{one, two, three, four, bomb, flag, cellImg, button};

    SDL_Rect placeImage{0, 0, CELL_WIDTH, CELL_HEIGHT};
    create_board(); 
    // Hack to get window to stay up
    SDL_Event e; bool quit = false; 
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type != SDL_MOUSEMOTION) {
                if (e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    int gridNum = (e.motion.y / CELL_HEIGHT * GRID_COLS + e.motion.x / CELL_WIDTH);
                    cells[gridNum].leftClick();
                    if (cells[gridNum].cell_layer[0] == 0) {
                        int visited[GRID_SIZE] = {0};
                        bfs(gridNum % GRID_COLS, gridNum / GRID_COLS, visited);
                    }
                } else if (e.button.button == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                    int gridNum = (e.motion.y / CELL_HEIGHT * GRID_COLS + e.motion.x / CELL_WIDTH);
                    cells[gridNum].rightClick();
                }
            } 
            // draw background    
            SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
            SDL_RenderClear(renderer);

            for (Cell &cell : cells) {
                placeImage.x = cell.x, placeImage.y = cell.y;
                if (cell.cell_layer[1]) {
                    SDL_RenderCopy(renderer, texture, &button, &placeImage);
                    if (cell.cell_layer[2]) SDL_RenderCopy(renderer, texture, &flag, &placeImage);
                } else {
                    SDL_Rect* pRect;
                    switch(cell.cell_layer[0]) {
                    case -1:
                        pRect = &bomb;
                        break;
                    case 0:
                        pRect = &cellImg;
                        break;
                    case 1:
                        pRect = &one;
                        break;
                    case 2:
                        pRect = &two;
                        break;
                    case 3:
                        pRect = &three;
                        break;
                    case 4:
                        pRect = &four;
                        break;
                    default:
                        return EXIT_FAILURE; 
                    }
                    SDL_RenderCopy(renderer, texture, pRect, &placeImage);
                }
            }
            // Show what we have drawn
            SDL_RenderPresent(renderer);
        } 
    }
    close();
    
    return 0;
}

bool init() {
    // Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0) 
    {
        printf(" SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    // Create window 
    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) < 0)
    {
        printf(" SDL Window or Renderer could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if ( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf(" SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
};

bool loadMedia() {
    // Loading success flag
    bool success = true;

    // Load splash image
    imageSurface = SDL_LoadBMP("main.bmp"); 
    // imageSurface = loadSurface("Button.png");
    if (imageSurface == NULL) {
        printf( "Unable to load image %s! ", SDL_GetError() );
        success = false;
    }
    return success;
}

void add_bombs() { 
    int cellNum;
    for (int i = 0; i < BOMBS; ++i) {
        cellNum = rand() % (GRID_ROWS*GRID_COLS);
        cells[cellNum].cell_layer[0] = -1;
    }
    int x,y;
    for (int i = 0; i < GRID_ROWS; ++i) {
        for (int j = 0; j < GRID_COLS; ++j) {
            // Increment adj cells
            cellNum = i*GRID_COLS+j;
            if (cells[cellNum].cell_layer[0] >= 0) continue;
            for (int k = 0; k < 8; ++k) {
                y = i + direc[k][0];      
                x = j + direc[k][1];
                if (0 <= x && x < GRID_COLS && 0 <= y && y < GRID_ROWS && cells[y*GRID_COLS+x].cell_layer[0] >= 0) {
                    ++cells[y*GRID_COLS+x].cell_layer[0];
                }
            }
        }
    }
}


void bfs(int c, int r, int visited[GRID_SIZE]) {
    int cellNum = r * GRID_COLS + c;
    if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS || visited[cellNum]) return;
    ++visited[cellNum];
    cells[cellNum].cell_layer[1] = 0;
    if (cells[cellNum].cell_layer[0] > 0) return;
    for (int i = 0; i < 8; ++i) {
        bfs(c + direc[i][0], r + direc[i][1], visited);
    }
}


void create_board() {
    for (int i = 0; i < GRID_ROWS; ++i) {
        for (int j = 0; j < GRID_COLS; ++j) {
            Cell cell(j*CELL_WIDTH, i*CELL_HEIGHT);
            // follow some logic
            cell.cell_layer[1] = 1; // button
            cells.push_back(cell);
        }
    }
    add_bombs();
}

void close() {
    // Deallocate surface
    SDL_FreeSurface( imageSurface );
    imageSurface = NULL;

    // Destroy window
    SDL_DestroyWindow( window );
    window = NULL;

    //Quit SDL Subsystems
    SDL_Quit();
}