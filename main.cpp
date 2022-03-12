#include <iostream>

#include "olcNoiseMaker.h"

std::atomic<double> dFrequencyOutput = 0.0;
const double dOctaveBaseFrequency = 110.0; // A2
const double d12thRootOf2 = pow(2.0, 1.0 / 12.0); // beta

// convert frequency into angular velocity
inline double w(double dHertz) {
	return dHertz * 2.0 * PI;
}

enum soundType {
	SINE_WAVE,
	SQUARE_WAVE,
	TRIANGLE_WAVE,
	SAW_WAVE,
	SAW_WAVE_COMPRESSED,
	RANDOM_NOISE
};

// "type" of sound
double osc(double dHertz, double dTime, soundType type) {
	switch (type) {
	case SINE_WAVE:
		return sin(w(dHertz) * dTime);
	case SQUARE_WAVE:
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;
	case TRIANGLE_WAVE:
		return asin(sin(w(dHertz) * dTime)) * 2.0 / PI;
	case SAW_WAVE:
	{
		double dOutput = 0.0;
		for (double n = 1.0; n < 10.0; ++n) {
			dOutput += sin(n * w(dHertz) * dTime) / n;
		}
		return dOutput * (2.0 / PI);		
	}		
	case SAW_WAVE_COMPRESSED:
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - PI / 2.0);
	case RANDOM_NOISE:
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;	
	}
	return 0;
}

double MakeNoise(double dTime) {
	double dOutput = osc(dFrequencyOutput, dTime, SQUARE_WAVE) * 0.5;	

	return dOutput;
}

int main()
{
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

	// print the keyboard:
	std::wcout << "| | S | | | F | G | | | J | K | L | |   |\n"
				  "| |___| | |___|___| | |___|___|___| |   |\n"
				  "| Z | X | C | V | B | N | M | , | . | / |\n"
				  "|___|___|___|___|___|___|___|___|___|___|\n"
				  "  A   B   C   D   E   F   G   A   B   C\n";
	
	// Add the above keyboard: ZSXCFVGBNJMK,L./, based on virtual key codes:
	const std::vector<int> keyboard = {0x5A, 0x53, 0x58, 0x43, 0x46, 0x56, 0x47, 0x42, 0x4E, 0x4A, 0x4D, 0x4B, VK_OEM_COMMA, 0x4C, VK_OEM_PERIOD, VK_OEM_2};

	while (true) {
		bool bKeyPressed = false;
		for (int k = 0; k < keyboard.size(); ++k) {			
			// detect the pressed key, i.e. the key whose highest bit is set
			if (GetAsyncKeyState(keyboard[k]) & 0x8000) {
				// play the note
				// formula: note_freq = base_note * (beta ^ k)
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				bKeyPressed = true;
			}
		}
		// if no keys are pressed, play nothing
		if (!bKeyPressed) {
			dFrequencyOutput = 0.0;
		}
	}
	
	return 0;
}