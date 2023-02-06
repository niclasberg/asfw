#pragma once
#include <cstdint>
#include <array>
#include <async/future.hpp>

template<std::uint32_t bufferSize, std::uint32_t sampleFreq, std::uint8_t NKeys>
class App
{
    inline static constexpr float pitches[] = {
        261.63, // C4
        277.18, // C#4
        293.66, // D4
        311.13, // D#4
        329.63, // E4
        349.23, // F4
        369.99, // F#4
        392.00, // G4
        415.30, // G#4
        440.00, // A4
        466.16, // A#4
        493.88, // B4
        523.25  // C5
    };

    static_assert(sizeof(pitches) < NKeys, "Too many ADC channels");

public:
    async::Future<void, void> auto run()
    {
        return async::whenAll(
            dacI2S.writeContinuous(i2sDma, audioBuffers_, 2*bufferSize, [this](std::uint16_t * buffer) {
                fillBuffer(buffer);
            }),

            rangeFinder.readRangeContinuous()
            | async::map([this](std::uint8_t range) {
                filter_.setCutoff(800.f * float(range) / 255.f + 10.f);
            }),

            async::repeat(async::delay(readAndProcessKeys(), 5))
        );
    }

    void fillAudioBuffer(std::uint16_t * buffer)
    {
        for (int i = 0; i < bufferSize; i += 1)
        {
            // The DAC expects each sample as a 16 bit integer in 2s complement.
            // We're running in stereo, so write each sample twice
            std::int16_t val = (std::int16_t) (30000.f * filter(oscillator()) * envelope());
            *(buffer++) = val;
            *(buffer++) = val;
        }
    }

    void processRangeValue(std::uint8_t range)
    {
        filter_.setCutoff(800.f * float(range) / 255.f + 10.f);
    }

    void processKeyInput(const std::array<float, NKeys> & inputs)
    {
        int maxIndex = -1;
        std::uint16_t maxValue = 0;
        for (int i = 0; i < NKeys; ++i)
        {
            std::uint16_t currentValue = inputs[i];

            if (currentValue > 0x64 && currentValue > maxValue)
            {
                maxValue = currentValue;
                maxIndex = i;
            }
        }

        if (maxIndex != -1)
        {
            if (!noteIsOn_)
            {
                envelope_.noteOn();
                noteIsOn_ = true;
            }

            oscillator_.setFrequency(pitches[maxIndex]);
            //oscillator_.setAmplitude(amplitudeScalingFactor * maxValue);
        }
        else
        {
            if (noteIsOn_)
            {
                envelope_.noteOff();
                noteIsOn_ = false;
            }
        }
    }

private:
    // Audio buffers
    

    // DSP
    Oscillator oscillator_(sampleFreq);
    Filter filter_{sampleFreq};
    Envelope envelope_{sampleFreq};

    bool noteIsOn_ = false;
};