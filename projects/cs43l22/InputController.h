#pragma once
#include "Envelope.h"
#include "Oscillator.h"
#include <async/transform.hpp>

template<class Adc, class Dma>
struct InputController
{
    inline static constexpr std::uint8_t numberOfNotes = Adc::numberOfChannels;
    inline static constexpr float amplitudeScalingFactor = 1.f / float(Adc::maxValue);
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

public:
    InputController(Adc & adc, Dma & dma, Envelope & envelope, Oscillator & oscillator)
    : keyInput_(adc)
    , keyInputDma_(dma)
    , envelope_(envelope)
    , oscillator_(oscillator)
    {

    }

    async::Sender auto update()
    {
        return keyInput_.read(keyInputDma_, (std::uint16_t *)data_)
        | async::transform([this]() {
            int maxIndex = -1;
            std::uint16_t maxValue = 0;
            for (int i = 0; i < numberOfNotes; ++i)
            {
                std::uint16_t currentValue = data_[i];

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

        });
    }
private:
    Adc & keyInput_;
    Dma & keyInputDma_;
    Envelope & envelope_;
    Oscillator & oscillator_;
    
    volatile std::uint16_t data_[numberOfNotes];
    bool noteIsOn_ = false;
};