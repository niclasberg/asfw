#include "Filter.h"
#include <cmath>
#include <cmath>

namespace 
{
	constexpr float PI = 3.14159265358979323846;
}

Filter::Filter(float sampleRate)
: sampleRate(sampleRate)
{
	updateCoeffs();
}

float Filter::operator()(float sample)
{
	float res = a0*sample + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
	x2 = x1;
	x1 = sample;
	y2 = y1;
	y1 = res;
	return res;
}

void Filter::setCutoff(float c)
{
	cutoff = c;
	updateCoeffs();
}

void Filter::setResonance(float r)
{
	resonance = r;
	updateCoeffs();
}

void Filter::updateCoeffs()
{
	float c = 1.0f / (tanf(PI * (cutoff / sampleRate)));
	float csq = c * c;
	float res = powf(10.0f, -(resonance * 0.05f));
	float q = sqrt(2.0f) * res;

	a0 = 1.0f / (1.0f + (q * c) + (csq));
	a1 = 2.0f * a0;
	a2 = a0;
	b1 = (2.0f * a0) * (1.0f - csq);
	b2 = a0 * (1.0f - (q * c) + csq);
}
