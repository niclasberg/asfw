#include "Envelope.h"

Envelope::Envelope(float sampleRate)
: sampleRate(sampleRate)
{
    setSustain(0.8);
    setAttack(0.1);
    setDecay(0.1);
    setRelease(0.3);
}

void Envelope::setAttack(float attack)
{
    attackDelta = 1.0f / (sampleRate * attack);
}

void Envelope::setSustain(float sustain)
{
    // TODO: update decayDelta and releaseDelta
    sustainLevel = sustain;
}

void Envelope::setDecay(float decay)
{
    decayDelta = (1.0f - sustainLevel) / (sampleRate * decay);
}

void Envelope::setRelease(float release)
{
    releaseDelta = sustainLevel / (sampleRate * release);
}

float Envelope::operator()()
{
    switch (state)
    {
    case State::IDLE:
    case State::SUSTAIN:
        break;
    case State::ATTACK:
        currentLevel += attackDelta;
        if(currentLevel >= 1.f)
        {
            currentLevel = 1.f;
            state = State::DECAY;
        }
        break;
    case State::DECAY:
        currentLevel -= decayDelta;
        if(currentLevel <= sustainLevel)
        {
            currentLevel = sustainLevel;
            state = State::SUSTAIN;
        }
        break;
    case State::RELEASE:
        currentLevel -= releaseDelta;
        if(currentLevel <= 0.f)
        {
            currentLevel = 0.f;
            state = State::IDLE;
        }
        break;
    }
    return currentLevel;
}