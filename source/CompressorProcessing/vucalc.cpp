#pragma once

#include "vucalc.h"
#include <cmath>



#ifndef PI
#define PI  3.1415926535897932384626433832795
#endif

namespace saola {
	VUmeter::VUmeter()
	{
		buffer = nullptr;
		vuRMS[0] = 0;
		vuOut[0] = 0;
		vuRMS[1] = 0;
		vuOut[1] = 0;
		numOfSamples = 0;
		sampleOfRates = 44100;
		a = 0;
	}

	VUmeter::~VUmeter()
	{
		delete[] buffer[0];
		delete[] buffer[1];
		delete[] buffer;
	}

	float** VUmeter::calculateRMSIndB(float** in, int numSamples)
	{
		for (int i = 0; i < 2; i++)
		{
			int samples = numSamples;

			float* ptrIn = in[i];
			float* ptrout = buffer[i];

			while (--samples >= 0)
			{
				vuRMS[i] = (*ptrIn++);
				vuRMS[i] = vuRMS[i] * vuRMS[i];
				vuRMS[i] *= a;
				vuRMS[i] = vuRMS[i] + ((1 - a) * vuOut[i]);
				vuOut[i] = vuRMS[i];
				(*ptrout++) = 20 * log10(sqrt(vuOut[i]));
			}
		}
		return buffer;
	}

	void VUmeter::set(double sampleRate, int numSamples, double tau)
	{
		sampleOfRates = sampleRate;
		numOfSamples = numSamples;
		buffer = new float * [2];
		buffer[0] = new float[numOfSamples];
		buffer[1] = new float[numOfSamples];

		double invSampleRate = 1. / sampleOfRates;
		double omega = 2.0 * PI * (1 / tau) * invSampleRate;
		a = omega / (1 + omega);
		
	}
}