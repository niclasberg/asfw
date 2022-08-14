#include "Oscillator.h"

float Oscillator::operator()()
{
    float val = 2.f * phase_ - 1.f;

    // Polyblep band limiting
    if (phase_ < dPhase_)
    {
        const float t = phase_ / dPhase_;
        val -= 2.f*t - t*t - 1.f;
    }
    else if (phase_ > (1.f - dPhase_))
    {
        const float t = (phase_ - 1.f) / dPhase_ + 1.f;
        val -= t*t;
    }

    phase_ += dPhase_;
    if (phase_ > 1.0f) phase_ -= 1.f;
    return amplitude_ * val;
}