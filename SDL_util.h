#ifndef SDL_UTIL_H
#define SDL_UTIL_H

#include <SDL.h>

const int SCREEN_WIDTH = 1897;
const int SCREEN_HEIGHT = 720;

// Start up SDL, create a *window* and a *renderer*
bool init(SDL_Window* &window, SDL_Renderer* &renderer);

void close(SDL_Window* &window, SDL_Renderer* &renderer) ;

#endif // SDL_UTIL_H