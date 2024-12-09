#pragma once


#include "base/source/fstreamer.h"
#include "DspFilters\Butterworth.h"
#include "compressor.h"

namespace saola {

	class channelSplit
	{
	public:
		channelSplit();
		~channelSplit();
		void init(double sampleRate, int numSamples, int numSplits = 3, int numChannels = 2);
		void freqSplitSetup();
		
		float*** multiplyAndFilter(float** in, int numSamples);
		void freqOut();

		void compressorSetup(double attack_ms, double release_ms, double ratio, double* threshold_dB, int type);
		void compressAndSumUp(float** out);
		void justSumUp(float** in);

		void dryWetAdjust(float** in, float** out, float amount);

		void clearFilters();
		void clearChannels();
		void clearCompressor();

		int getNumOfSplits() { return numOfSplits; };

		void compensate(float** out, double howMuchdB, bool forInsideChannels = false);

		void isDelta(bool Enabled);
	protected:
		float*** channelArray;

		int numOfChannels, numOfSplits, numOfSamples;
		double sampleOfRates;

		Dsp::SimpleFilter <Dsp::Butterworth::LowPass<4>, 2>** filterLP;
		Dsp::SimpleFilter <Dsp::Butterworth::BandPass<2>, 2>*** filterBP;
		Dsp::SimpleFilter <Dsp::Butterworth::HighPass<4>, 2>** filterHP;

		compress* compressArray;
		double* middleFreqs;

		bool isDeltaEnabled = false;
		double mThreshold = 0;
		
	};

}