#pragma once

class Filter {
public:
	explicit Filter(float sampleRate);

	float operator()(float input);

	void setCutoff(float c);
	void setResonance(float r);
private:
	void updateCoeffs();

	float cutoff = 1000;
	float resonance = 1;

	const float sampleRate;
	float x1 = 0;
	float x2 = 0;
	float y1 = 0;
	float y2 = 0;
	float a0, a1, a2, b1, b2;
};

