#include <SDL.h>
#include "SDL_util.h"
#include "ltexture.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[]) {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    init(window, renderer);

    LTexture keyboardTexture;
    keyboardTexture.loadFromFile("graphic_files/keyboardClipArt.png", renderer);

    const std::vector<SDL_Scancode> keyboardScancodes = {
        SDL_SCANCODE_Q, // C1
        SDL_SCANCODE_2, // C#1
        SDL_SCANCODE_W, // D1
        SDL_SCANCODE_3, // D#1
        SDL_SCANCODE_E, // E1
        SDL_SCANCODE_R, // F1
        SDL_SCANCODE_5, // F#1
        SDL_SCANCODE_T, // G1
        SDL_SCANCODE_6, // G#1
        SDL_SCANCODE_Y, // A2
        SDL_SCANCODE_7, // A#2
        SDL_SCANCODE_U, // B2
        SDL_SCANCODE_I, // C2
        SDL_SCANCODE_9, // C#2
        SDL_SCANCODE_O, // D2
        SDL_SCANCODE_0, // D#2
        SDL_SCANCODE_P, // E2
        SDL_SCANCODE_Z, // F2
        SDL_SCANCODE_S, // F#2
        SDL_SCANCODE_X, // G2
        SDL_SCANCODE_D, // G#2
        SDL_SCANCODE_C, // A3
        SDL_SCANCODE_F, // A#3
        SDL_SCANCODE_V, // B3
        SDL_SCANCODE_B, // C3
        SDL_SCANCODE_H, // C#3
        SDL_SCANCODE_N, // D3
        SDL_SCANCODE_J, // D#3
        SDL_SCANCODE_M, // E3
        SDL_SCANCODE_COMMA, // F3
        SDL_SCANCODE_L, // F#3
        SDL_SCANCODE_PERIOD, // G3
        SDL_SCANCODE_SEMICOLON, // G#3
        SDL_SCANCODE_SLASH, // A4
        SDL_SCANCODE_APOSTROPHE, // A4#
    };

    const std::unordered_map<SDL_Scancode, std::string> scancode_to_keyname{
        {SDL_SCANCODE_Q, "Q"},
        {SDL_SCANCODE_2, "2"},
        {SDL_SCANCODE_W, "W"},
        {SDL_SCANCODE_3, "3"},
        {SDL_SCANCODE_E, "E"},
        {SDL_SCANCODE_R, "R"},
        {SDL_SCANCODE_5, "5"},
        {SDL_SCANCODE_T, "T"},
        {SDL_SCANCODE_6, "6"},
        {SDL_SCANCODE_Y, "Y"},
        {SDL_SCANCODE_7, "7"},
        {SDL_SCANCODE_U, "U"},
        {SDL_SCANCODE_I, "I"},
        {SDL_SCANCODE_9, "9"},
        {SDL_SCANCODE_O, "O"},
        {SDL_SCANCODE_0, "0"},
        {SDL_SCANCODE_P, "P"},
        {SDL_SCANCODE_Z, "Z"},
        {SDL_SCANCODE_S, "S"},
        {SDL_SCANCODE_X, "X"},
        {SDL_SCANCODE_D, "D"},
        {SDL_SCANCODE_C, "C"},
        {SDL_SCANCODE_F, "F"},
        {SDL_SCANCODE_V, "V"},
        {SDL_SCANCODE_B, "B"},
        {SDL_SCANCODE_H, "H"},
        {SDL_SCANCODE_N, "N"},
        {SDL_SCANCODE_J, "J"},
        {SDL_SCANCODE_M, "M"},
        {SDL_SCANCODE_COMMA, "COMMA"},
        {SDL_SCANCODE_L, "L"},
        {SDL_SCANCODE_PERIOD, "PERIOD"},
        {SDL_SCANCODE_SEMICOLON, "SEMICOLON"},
        {SDL_SCANCODE_SLASH, "SLASH"},
        {SDL_SCANCODE_APOSTROPHE, "APOSTROPHE"}
    };

    std::vector<LTexture> keyTextures(keyboardScancodes.size());
    SDL_Color colorKey{0xFF, 0xFF, 0xFF, 0xFF};

    for (size_t i = 0; i < keyTextures.size(); ++i) {
        std::string path = "graphic_files/individual_keys/" + scancode_to_keyname.at(keyboardScancodes[i]) + "pressed.png";
        keyTextures[i].loadFromFile(path, renderer, &colorKey);
    }

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        
        SDL_Rect bottomViewport;
        bottomViewport.x = 0;
        bottomViewport.y = SCREEN_HEIGHT / 2;
        bottomViewport.w = SCREEN_WIDTH;
        bottomViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport(renderer, &bottomViewport);
        
        keyboardTexture.render(renderer, 0, 0);

        int numkeys = 0;
        
        const Uint8 *currentKeyState = SDL_GetKeyboardState(&numkeys);

        for (size_t i = 0; i < keyboardScancodes.size(); ++i) {
            if (currentKeyState[keyboardScancodes[i]]) {
                keyTextures[i].render(renderer, 0, 0);
            }
        }

        SDL_Delay(10);

        SDL_RenderPresent(renderer);
    }

    close(window, renderer);
    return 0;
}