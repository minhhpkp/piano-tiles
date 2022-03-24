#include <iostream>
#include "olcNoiseMaker.h"
#include <SDL.h>
#include "synthesizer.h"
#include <vector>

const double dOctaveBaseFrequency = 220.0; // A2
const double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // beta

synth::instrument *voice = nullptr;
std::vector<synth::note> vecNotes;

double MakeNoise(double dTime) {
	double dOutput = 0.0;
	for (const auto& n: vecNotes) {
		dOutput += voice->sound(n, dTime) * 0.4;
	}
	return dOutput;
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

	voice = new synth::instrBell();

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
			synth::note curNote;
			curNote.dFreq = dOctaveBaseFrequency * pow(d12thRootOf2, k);
			// the current note is already active 
			auto noteFound = std::find_if(vecNotes.begin(), vecNotes.end(), [&curNote](const synth::note& n) {
				return n.dFreq == curNote.dFreq;
			});

			if (GetAsyncKeyState(keyboard[k]) & 0x8000) {
				// if user starts playing new note
				if (noteFound == vecNotes.end()) {
					curNote.dTimeOn = dTimeNow;
					curNote.active = true;
					vecNotes.push_back(curNote);	
				}
			}
			else {
				// the key is not pressed but it's in vecNotes
				if (noteFound != vecNotes.end()) {
					// put it in release mode if it's still active
					if (noteFound->active) {
						noteFound->dTimeOff = dTimeNow;
						noteFound->active = false;
					}
					// or erase it from vecNotes if its release phase has ended
					else if (dTimeNow - noteFound->dTimeOff > voice->env.dReleaseTime) {
						vecNotes.erase(noteFound);
					}
				}
			}
		}
	}
	
	return 0;
}