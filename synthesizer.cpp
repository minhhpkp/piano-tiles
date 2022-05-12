#include <cstdlib>
#include "olcNoiseMaker.h"
#include "synthesizer.h"

namespace synth {
    // get angular velocity from a frequency
    inline double w(double dHertz) {
        return 2 * PI * dHertz;
    }
    double osc(double dHertz, double dTime, soundType type, double dLFOAmplitude /*= 0.0*/, double dLFOFrequency /*= 0.0*/) {
        double dFrequency = w(dHertz) * dTime + dLFOAmplitude * dHertz * dTime * sin(w(dLFOFrequency) * dTime);
        
        switch (type) {
            case SINE_WAVE:
                return sin(dFrequency);
            case SQUARE_WAVE:
                return sin(dFrequency) > 0.0 ? 1.0 : -1.0;
            case TRIANGLE_WAVE:
                return asin(sin(dFrequency)) / (PI / 2);
            case SAW_WAVE:
                {
                    double dOutput = 0.0;
                    for (double n = 1.0; n < 100.0; ++n) {
                        dOutput += sin(n * dFrequency) / n;
                    }
                    return dOutput * (2.0 / PI);
                }
            case SAW_WAVE_COMPRESSED:
                {
                    dHertz = dFrequency / dTime / 2 / PI;
		            return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - PI / 2.0);
                }
            case RANDOM_NOISE:
                return 2.0 * ((double) rand() / (double)RAND_MAX) - 1.0;
        }

        return 0.0;
    }
    
    double note::getFreq() const {
        return dOctaveBaseFrequency * pow(d12thRootOf2, id);
    }

    double sEnvelopeADSR::getAmplitude(double dTime, double dTriggeredOn, double dTriggeredOff) {
        double dAmplitude = 0.0;

        if (dTriggeredOn > dTriggeredOff) {
            double dLifeTime = dTime - dTriggeredOn;
            if (dLifeTime >= 0 && dLifeTime <= dAttackTime) {
                dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
            }
            else if (dLifeTime >= dAttackTime && dLifeTime <= dAttackTime + dDecayTime) {
                dAmplitude = dStartAmplitude - (dLifeTime - dAttackTime) / dDecayTime * (dStartAmplitude - dSustainAmplitude);
            }
            else if (dLifeTime >= dAttackTime + dDecayTime) {
                dAmplitude = dSustainAmplitude;
            }
        }
        // If the note is off and is still in its Release phase
        else if (dTime - dTriggeredOff <= dReleaseTime) {
            double dActiveTime = dTriggeredOff - dTriggeredOn;
            double dReleaseAmplitude = 0.0;
            
            // The key is released during the Attack phase
            if (dActiveTime >= 0 && dActiveTime <= dAttackTime) {
                dReleaseAmplitude = dActiveTime / dAttackTime * dStartAmplitude;
            }
            // The key is released during the Decay phase
            else if (dActiveTime >= dAttackTime && dActiveTime <= dAttackTime + dDecayTime) {
                dReleaseAmplitude = dStartAmplitude - (dActiveTime - dAttackTime) / dDecayTime * (dStartAmplitude - dSustainAmplitude);
                
            }
            // The key is released during the Sustain phase
            else if (dActiveTime >= dActiveTime + dDecayTime) {
                dReleaseAmplitude = dSustainAmplitude;
            }
            dAmplitude = dReleaseAmplitude - (dTime - dTriggeredOff) / dReleaseTime * dReleaseAmplitude;
        }

        if (dAmplitude <= 0.0001) {
            dAmplitude = 0.0;
        }
        return dAmplitude;
    }
    
    instrBell::instrBell() {        
        env.dAttackTime = 0.01;
        env.dDecayTime = 1.0;
        env.dStartAmplitude = 1.0;
        env.dSustainAmplitude = 0.0;
        env.dReleaseTime = 1.0;
    }
    double instrBell::sound(const note& n, double dTime) {
        double dOutput = env.getAmplitude(dTime, n.dTimeOn, n.dTimeOff) * (
            + 1.0 * osc(n.getFreq() * 2.0, dTime, SINE_WAVE, 5.0, 0.001)
            + 0.5 * osc(n.getFreq() * 3.0, dTime, SINE_WAVE)
            + 0.25 * osc(n.getFreq() * 4.0, dTime, SINE_WAVE)
        );
        return dOutput / 1.75;
    }

    instrHarmonica::instrHarmonica() {
        env.dAttackTime = 0.05;
        env.dDecayTime = 1.0;
        env.dStartAmplitude = 1.0;
        env.dSustainAmplitude = 0.9;
        env.dReleaseTime = 0.1;
    }
    double instrHarmonica::sound(const note& n, double dTime) {
        double dOutput = env.getAmplitude(dTime, n.dTimeOn, n.dTimeOff) * (
            + 1.0 * osc(n.getFreq(), dTime, SQUARE_WAVE, 5.0, 0.001)
            + 0.5 * osc(n.getFreq() * 1.5, dTime, SQUARE_WAVE)
            + 0.25 * osc(n.getFreq() * 2.0, dTime, SQUARE_WAVE)
            + 0.05 * osc(0, dTime, RANDOM_NOISE)
        );
        return dOutput / 1.8;
    }
}