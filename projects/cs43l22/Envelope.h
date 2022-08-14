#pragma once

class Envelope
{
public:
    enum class State
    {
        IDLE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    Envelope(float sampleRate);

    void noteOn() { state = State::ATTACK; }
    void noteOff() { state = State::RELEASE; }

    /** 
     * Set attack time
     * 
     * @param attack duration (in seconds) of the attack phase
     **/
    void setAttack(float attack);

    /**
     * Set sustain value
     * 
     * @param sustain Output level [0, 1] during sustain phase
     **/
    void setSustain(float sustain);
    void setDecay(float decay);
    void setRelease(float release);

    float operator()();

private:

    State state = State::IDLE;
    float sampleRate;
    float currentLevel = 0.f;
    float attackDelta;
    float decayDelta;
    float sustainLevel;
    float releaseDelta;
};