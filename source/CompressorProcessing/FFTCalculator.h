/*
  ==============================================================================

    FFTCalculator.h
    Created: 21 Oct 2021 8:16:09pm
    Author:  kosty

  ==============================================================================
*/

#pragma once

#ifndef PI
#define PI  3.1415926535897932384626433832795
#endif

#include "base/source/fstreamer.h"
#include "fftw3.h"
#include "circularBuffer.h"

namespace saola {

	class FFTCalculator
	{
	public:
		FFTCalculator();
		~FFTCalculator();
		void init(int bufferSize, int numChannels = 2, double sampleRate = 48000);

		void addBuffer(float** inputBuffer, int numSamples) { mCircularBuffer->addBuffer(inputBuffer, numSamples); };
		float* getBuffer(); // mono buffer of amplitudes
		float* getFreqs(); // mono buffer of freqs

		int getNumChannels() { return mNumChannels; };
		int getSize() { return mSize; };

		void reset();
	protected:

		float* calculateAmplitudeOfComplexArray(fftw_complex* complexArray);
		void makeItMono(float** buffer);
		void applyHanningWindow(double* buffer); //monobuffer
		void setSpectrumSpeed(float ReleaseTime = RELEASETIME);

		circularBuffer* mCircularBuffer;
		float* mOutputBuffer[2];

		float* mFreqs = nullptr;

		int mNumChannels, mSize;
		double mSampleRate;

		float testMax = 0;

		double* fftin;
		fftw_complex* fftout;
		fftw_plan p;
	};

}