/*
  ==============================================================================

    channelSplit_wShelf.h
    Created: 19 Nov 2021 4:53:46pm
    Author:  kosty

  ==============================================================================
*/

#pragma once

#include "base/source/fstreamer.h"
#include "DspFilters\Butterworth.h"
#include "compressor.h"

namespace saola {

	class channelSplit_wShelf
	{
	public:
		channelSplit_wShelf();
		~channelSplit_wShelf();
		void init(double sampleRate, int numSamples, int numSplits = 3, int numChannels = 2);
		void freqSplitSetup();



		void shelfFilterSetup(float** rawInput);
		void shelfFilterprocess(float** in);
		float* calcGainReduction();







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

		Dsp::SimpleFilter <Dsp::Butterworth::LowShelf<4>, 2> filterLS;
		Dsp::SimpleFilter <Dsp::Butterworth::BandShelf<4>, 2>* filterBS;
		Dsp::SimpleFilter <Dsp::Butterworth::HighShelf<4>, 2> filterHS;

		compress* compressArray;
		double* middleFreqs;

		float* gainReduction;

		bool isDeltaEnabled = false;
		double mThreshold = 0;

	};

}