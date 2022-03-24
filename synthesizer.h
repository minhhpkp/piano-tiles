#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

namespace synth {
    enum soundType {
        SINE_WAVE,
        SQUARE_WAVE,
        TRIANGLE_WAVE,
        SAW_WAVE,
        SAW_WAVE_COMPRESSED,
        RANDOM_NOISE
    };

    struct note {
        double dFreq = 0.0;
        double dTimeOn = 0.0;
        double dTimeOff = 0.0;
        double dInitAmplitude = 0.0;
        bool active = false;
    };

    inline double w(double dHertz);
    double osc(double dHertz, double dTime, soundType type, double dLFOAmplitude = 0.0, double dLFOFrequency = 0.0);
    struct sEnvelopeADSR {
        double dAttackTime = 0.1;
        double dDecayTime = 1.0;
        double dStartAmplitude = 1.0;
        double dSustainAmplitude = 0.8;
        double dReleaseTime = 0.1;

        double getAmplitude(double dTime, double dTriggeredOn, double dTriggeredOff);
    };

    struct instrument {
        sEnvelopeADSR env;
        virtual double sound(const note& n, double dTime) = 0;
    };

    struct instrBell : public instrument {
        instrBell();
        double sound(const note& n, double dTime);
    };    

    struct instrHarmonica : public instrument {
        instrHarmonica();
        double sound(const note& n, double dTime);
    };

}

#endif // SYNTHESIZER_H