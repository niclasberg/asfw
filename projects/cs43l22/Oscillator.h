#pragma once

class Oscillator
{
public:
    explicit Oscillator(float sampleRate) : sampleRate_(sampleRate)
    {
        setFrequency(400.f);
    }

    void setFrequency(float frequency) { dPhase_ = frequency / sampleRate_; }
    void setAmplitude(float amplitude) { amplitude_ = amplitude; }

    float operator()();

private:
    float sampleRate_;
    float dPhase_;
    float phase_ = 0.f;
    float amplitude_ = 1.0f;
};