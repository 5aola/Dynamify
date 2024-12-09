#pragma once

#include "base/source/fstreamer.h"
#include "vucalc.h"
#include "paramids.h"

using namespace Steinberg;

namespace saola {
	

	class compress
	{
	public:
		compress();
		~compress();

		void init(int numSamples, double sampleRate, double tau, int numChannels = 2);
		void setup(double attackTimeMS, double releaseTimeMS, double kratio, double thresHolddB, int type = tCompress);
		float* compProcess(float** in, int numSamples);
		

	protected:

		float* gainReduction;
		int mNumChannels;
		float attackTime, releaseTime, ratio, thresdB;
		float sRate, *mGain;
		VUmeter* VU;
		int mType;

	};

}