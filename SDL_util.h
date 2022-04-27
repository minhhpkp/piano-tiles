#ifndef SDL_UTIL_H
#define SDL_UTIL_H

#include <SDL.h>

class Screen{
    public:
        const int SCREEN_WIDTH = 1897;
        const int SCREEN_HEIGHT = 720;

        // Start up SDL, create a *window* and a *renderer*
        bool init(SDL_Window* &window, SDL_Renderer* &renderer);

        void close(SDL_Window* &window, SDL_Renderer* &renderer);

        Screen(int scrn_w = 640, int scrn_h = 480): SCREEN_WIDTH(scrn_w), SCREEN_HEIGHT(scrn_h){}
};

#endif // SDL_UTIL_H