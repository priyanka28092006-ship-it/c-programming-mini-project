#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 800

void set_tile_color(SDL_Renderer* renderer, int value)
{
    switch(value)
    {
        case 2:    SDL_SetRenderDrawColor(renderer, 255, 182, 193, 255); break;
        case 4:    SDL_SetRenderDrawColor(renderer, 207, 151, 113, 255); break;
        case 8:    SDL_SetRenderDrawColor(renderer, 242, 177, 121, 255); break;
        case 16:   SDL_SetRenderDrawColor(renderer, 245, 149, 99, 255); break;
        case 32:   SDL_SetRenderDrawColor(renderer, 246, 124, 95, 255); break;
        case 64:   SDL_SetRenderDrawColor(renderer, 246, 94, 59, 255); break;
        case 128:  SDL_SetRenderDrawColor(renderer, 237, 207, 114, 255); break;
        case 256:  SDL_SetRenderDrawColor(renderer, 237, 204, 97, 255); break;
        case 512:  SDL_SetRenderDrawColor(renderer, 237, 200, 80, 255); break;
        case 1024: SDL_SetRenderDrawColor(renderer, 237, 197, 63, 255); break;
        case 2048: SDL_SetRenderDrawColor(renderer, 237, 194, 46, 255); break;
        default:   SDL_SetRenderDrawColor(renderer, 60, 58, 50, 255); break;
    }
}

void render_board(SDL_Renderer* renderer, int board[4][4], TTF_Font* font)
{
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    int currentScore = score(board);

    char scoreText[50];
    sprintf(scoreText, "Score: %d", currentScore);

    SDL_Color white = {255,255,255};

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_Rect scoreRect;
    scoreRect.w = scoreSurface->w;
    scoreRect.h = scoreSurface->h;
    scoreRect.x = (WINDOW_WIDTH - scoreRect.w) / 2;
    scoreRect.y = 30;

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    int GRID_COUNT = 4;
    int GAP = 10;
    int topOffset = 120;
    int gridWidth = 600;

    int tileSize = (gridWidth - (GRID_COUNT + 1) * GAP) / GRID_COUNT;
    int leftOffset = (WINDOW_WIDTH - gridWidth) / 2;

    for(int row = 0; row < GRID_COUNT; row++)
    {
        for(int col = 0; col < GRID_COUNT; col++)
        {
            SDL_Rect tile;

            tile.x = leftOffset + GAP + col * (tileSize + GAP);
            tile.y = topOffset + GAP + row * (tileSize + GAP);
            tile.w = tileSize;
            tile.h = tileSize;

            if(board[row][col] == 0)
                SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
            else
                set_tile_color(renderer, board[row][col]);

            SDL_RenderFillRect(renderer, &tile);

            if(board[row][col] != 0)
            {
                char num[10];
                sprintf(num, "%d", board[row][col]);

                SDL_Surface* surface = TTF_RenderText_Solid(font, num, white);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                SDL_Rect textRect;
                textRect.w = surface->w;
                textRect.h = surface->h;
                textRect.x = tile.x + (tile.w - textRect.w) / 2;
                textRect.y = tile.y + (tile.h - textRect.h) / 2;

                SDL_RenderCopy(renderer, texture, NULL, &textRect);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }
}

int main()
{
    int board[4][4];

    srand(time(NULL));
    initialize_board(board);
    add_random_tile(board);
    add_random_tile(board);

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 50);

    SDL_Window* window = SDL_CreateWindow(
        "2048",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = 0;

            if(event.type == SDL_KEYDOWN)
            {
                int moved = 0;

                switch(event.key.keysym.sym)
                {
                    case SDLK_w: moved = move_up(board); break;
                    case SDLK_a: moved = move_left(board); break;
                    case SDLK_s: moved = move_down(board); break;
                    case SDLK_d: moved = move_right(board); break;
                }

                if(moved)
                    add_random_tile(board);
            }
        }

        render_board(renderer, board, font);

        if(is_game_over(board))
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);

            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);

            SDL_Color white = {255,255,255};
            SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER", white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect textRect;
            textRect.w = surface->w;
            textRect.h = surface->h;
            textRect.x = (WINDOW_WIDTH - textRect.w) / 2;
            textRect.y = (WINDOW_HEIGHT - textRect.h) / 2;

            SDL_RenderCopy(renderer, texture, NULL, &textRect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}