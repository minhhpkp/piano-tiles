#include "SDL_util.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>


// Start up SDL, create a *window* and a *renderer*
bool Screen::init(SDL_Window* &window, SDL_Renderer* &renderer) {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialise SDL. SDL Error: %s\n", SDL_GetError());
        success = false;
    } else {
        // Set texture filtering to linear (why necessary?)
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ))
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

        window = SDL_CreateWindow("Piano Tiles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            printf("Failed to create window. SDL Error: %s\n", SDL_GetError());
            success  = false;
        } else {
            // Create hardware-accelerated, vsynced renderer for window
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (renderer == nullptr) {
                printf("Failed to create renderer. SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                // Set intial color for drawing shape and clearing screen as white
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("Failed to initialise SDL_image. SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("Failed to initialise SDL_ttf! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void Screen::close(SDL_Window* &window, SDL_Renderer* &renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;

    // Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}