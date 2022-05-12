#include <iostream>
#include "olcNoiseMaker.h"
#include <SDL.h>
#include "synthesizer.h"
#include <vector>

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

	voice = new synth::instrHarmonica();

	// print the keyboard:
	std::wcout << "| |   | | |   |   | | |   |   |   | |   |\n"
				  "| | S | | | F | G | | | J | K | L | |   |\n"
				  "| |___| | |___|___| | |___|___|___| |   |\n"
				  "|   |   |   |   |   |   |   |   |   |   |\n"
				  "| Z | X | C | V | B | N | M | , | . | / |\n"
				  "|___|___|___|___|___|___|___|___|___|___|\n"
				  "  A   B   C   D   E   F   G   A   B   C\n";
	
	// Add the above keyboard: ZSXCFVGBNJMK,L./, based on virtual key codes:
	const std::vector<int> keyboard = {0x5A, 0x53, 0x58, 0x43, 0x46, 0x56, 0x47, 0x42, 0x4E, 0x4A, 0x4D, 0x4B, VK_OEM_COMMA, 0x4C, VK_OEM_PERIOD, VK_OEM_2};

	while (true) {
		double dTimeNow = sound.GetTime();
		for (int k = 0; k < int(keyboard.size()); ++k) {
			auto noteFound = std::find_if(vecNotes.begin(), vecNotes.end(), [&k](const synth::note& n) {
				return n.id == k;
			});

			// if the key is pressed
			if (GetAsyncKeyState(keyboard[k]) & 0x8000) {
				// if the note has not been active yet, start playing it
				if (noteFound == vecNotes.end()) {
					synth::note newNote;
					newNote.id = k;
					newNote.dTimeOn = dTimeNow;
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
	}
	
	return 0;
}