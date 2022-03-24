#include <iostream>
#include "olcNoiseMaker.h"
#include <SDL.h>
#include "synthesizer.h"

std::atomic<double> dFrequencyOutput = 0.0;
const double dOctaveBaseFrequency = 220.0; // A2
const double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // beta

synth::instrument *voice = nullptr;

double MakeNoise(double dTime) {
	double dOutput = voice->sound(dFrequencyOutput, dTime) * 0.4;
	return dOutput;
}

int main(int argc, char* argv[]) {
	// Get all sound hardware
	std::vector<std::wstring> devices = olcNoiseMaker<short>::Enumerate();

	// Display findings
	for (auto d : devices) 
		std::wcout << "Found Output Device: " << d << std::endl;
	std::wcout << "Using Device: " << devices[0] << std::endl;

	// Create sound machine!!
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction(MakeNoise);

	voice = new synth::instrBell();

	// print the keyboard:
	std::wcout << "| | S | | | F | G | | | J | K | L | |   |\n"
				  "| |___| | |___|___| | |___|___|___| |   |\n"
				  "| Z | X | C | V | B | N | M | , | . | / |\n"
				  "|___|___|___|___|___|___|___|___|___|___|\n"
				  "  A   B   C   D   E   F   G   A   B   C\n";
	
	// Add the above keyboard: ZSXCFVGBNJMK,L./, based on virtual key codes:
	const std::vector<int> keyboard = {0x5A, 0x53, 0x58, 0x43, 0x46, 0x56, 0x47, 0x42, 0x4E, 0x4A, 0x4D, 0x4B, VK_OEM_COMMA, 0x4C, VK_OEM_PERIOD, VK_OEM_2};
	int currentKey = -1;		
	while (true) {
		bool bKeyPressed = false;	
		for (int k = 0; k < keyboard.size(); ++k) {			
			// detect the pressed key, i.e. the key whose highest bit is set
			if (GetAsyncKeyState(keyboard[k]) & 0x8000) {
				// play the note whose frequency is: note_freq = base_note * (beta ^ k)				
				bKeyPressed = true;
				// if the user changes key:
				if (currentKey != k) {
					voice->env.noteOn(sound.GetTime());
					dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
					currentKey = k;
				}				
			}
		}	
		if (!bKeyPressed) {
			// if the key has just been released
			if (currentKey != -1) {
				// play the release phase
				voice->env.noteOff(sound.GetTime());
				currentKey = -1;
			}															
		}
	}
	
	return 0;
}