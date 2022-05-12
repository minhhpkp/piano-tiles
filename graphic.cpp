#include <SDL.h>
#include "SDL_util.h"
#include "ltexture.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "box.hpp"
#include <deque>

// add sound to the UI
#include "synthesizer.h"
#include "olcNoiseMaker.h"
#include <algorithm>


synth::instrument *voice = nullptr;
std::vector<synth::note> vecNotes;

std::mutex muxNotes;

double MakeNoise(double dTime) {
	std::unique_lock<std::mutex> lm(muxNotes);
	double dOutput = 0.0;
	for (const auto& n: vecNotes) {
		dOutput += voice->sound(n, dTime);
	}
	return dOutput / vecNotes.size();
}

int main(int argc, char* argv[]) {
    // Get all sound hardware
	std::vector<std::wstring> devices = olcNoiseMaker<short>::Enumerate();

	// Display findings
	for (auto d : devices) 
		std::wcout << "Found Output Device: " << d << std::endl;
	std::wcout << "Using Device: " << devices[0] << '\n' << std::endl;

	// Create sound machine!!
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction(MakeNoise);

    // Let the user choose voice
    while (voice == nullptr) {
        std::cout << "Choose voice, Harmonica or Bell (H or B): ";
        char c;
        std::cin >> c;
        if (c == 'H' || c == 'h') {
            voice = new synth::instrHarmonica();
            break;
        }
        else if (c == 'B' || c == 'b') {
            voice = new synth::instrBell();
            break;
        }
    }

    // Create windows
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    Screen s(1897, 720);

    s.init(window, renderer);

    LTexture keyboardTexture;
    keyboardTexture.loadFromFile("graphic_files/keyboardClipArt.png", renderer);

    const int KEYBOARD_SIZE = 12 * 3;

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
        SDL_SCANCODE_RSHIFT, // B4
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
        {SDL_SCANCODE_APOSTROPHE, "APOSTROPHE"},
        {SDL_SCANCODE_RSHIFT, "RSHIFT"}
    };

    std::vector<LTexture> keyTextures(KEYBOARD_SIZE);
    SDL_Color colorKey{0xFF, 0xFF, 0xFF, 0xFF};
    // loading each key's image into its texture
    for (size_t i = 0; i < keyTextures.size(); ++i) {
        std::string path = "graphic_files/individual_keys/" + scancode_to_keyname.at(keyboardScancodes[i]) + "pressed.png";
        keyTextures[i].loadFromFile(path, renderer, &colorKey);
    }

    // setting the width and the x coordinate of the box correspond to the ith note
    Box boxes[KEYBOARD_SIZE] = {
        {renderer, 371, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 371 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 24, 100, {0, 0, 0xff, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 371 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2 + 26 + 2, 0, 32, 100, {0, 0, 0xff, 0xff}},
        /* 7 pixels */
        {renderer, 740, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 740 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 24, 100, {0, 0, 0xff, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 740 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2 + 26 + 2, 0, 32, 100, {0, 0, 0xff, 0xff}},
        /* 7 pixels */
        {renderer, 1109, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 1109 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2, 0, 32, 100, {0xff, 0, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2, 0, 29, 100, {0, 0, 0xff, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2, 0, 24, 100, {0, 0, 0xff, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2, 0, 26, 100, {0, 0xff, 0, 0xff}},
        {renderer, 1109 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 32 + 2 + 32 + 2 + 26 + 2 + 29 + 2 + 26 + 2 + 24 + 2 + 26 + 2, 0, 32, 100, {0, 0, 0xff, 0xff}}
    };

    // viewport for rendering the keyboard
    SDL_Rect bottomViewport{0, s.SCREEN_HEIGHT - keyboardTexture.getHeight(), s.SCREEN_WIDTH, keyboardTexture.getHeight()};

    // viewport for rendering the falling boxes
    SDL_Rect topViewport{0, 0, s.SCREEN_WIDTH, s.SCREEN_HEIGHT - keyboardTexture.getHeight()};

    bool quit = false;
    SDL_Event e;
    std::deque<Box> q;
    
    // main loop
    while (!quit) {
        // handle quit event
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            for (auto &box: q) {
                box.move(s.SCREEN_HEIGHT);
            }
        }

        // dequeue disappeared boxes
        while (!q.empty() && q.front().getH() == 0) {
            q.pop_front();
        }
        
        // clear the screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        
        // render the keyboard
        SDL_RenderSetViewport(renderer, &bottomViewport);
        keyboardTexture.render(renderer, 0, 0);

        const Uint8 *currentKeyState = SDL_GetKeyboardState(nullptr);
        double dTimeNow = sound.GetTime();
        for (size_t i = 0; i < keyboardScancodes.size(); ++i) {
            auto noteFound = std::find_if(vecNotes.begin(), vecNotes.end(), [&i](const synth::note& n) {
                return n.id == i;
            });
            
            // if the key is pressed
            if (currentKeyState[keyboardScancodes[i]]) {
                keyTextures[i].render(renderer, 0, 0);
                q.push_back(boxes[i]);

                // if the note has not been active yet, start playing it
                if (noteFound == vecNotes.end()) {
                    synth::note newNote(i, dTimeNow);
                    vecNotes.push_back(newNote);
                }
                // if the note is still being active, do nothing as the user simply keeps holding the key
            }
            else {
				// if the key is not pressed anymore and the note is still being active
				if (noteFound != vecNotes.end()) {
					// either put it in release mode
					if (noteFound->dTimeOff < noteFound->dTimeOn) {
						noteFound->dTimeOff = dTimeNow;
					}
					// or erase it if its release mode has finished
					else if (dTimeNow - noteFound->dTimeOff > voice->env.dReleaseTime) {
						vecNotes.erase(noteFound);
					}					
				}
			}
        }
        
        SDL_RenderSetViewport(renderer, &topViewport);
        for (auto &box: q) {
            box.move(s.SCREEN_HEIGHT);
            box.render();
        }

        SDL_RenderPresent(renderer);
    }

    s.close(window, renderer);
    return 0;
}