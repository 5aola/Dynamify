/*
  ==============================================================================

    channelSplit_wShelf.cpp
    Created: 19 Nov 2021 4:53:46pm
    Author:  kosty

  ==============================================================================
*/

#include "channelSplit_wShelf.h"
#include <cmath>

saola::channelSplit_wShelf::channelSplit_wShelf()
{
	channelArray = nullptr;
	filterLP = nullptr;
	filterBP = nullptr;
	filterHP = nullptr;
	middleFreqs = nullptr;
	compressArray = nullptr;
	numOfSplits = 3;
	numOfChannels = 2;
	numOfSamples = 0;
	sampleOfRates = 44100;

}

saola::channelSplit_wShelf::~channelSplit_wShelf()
{
	clearChannels();
	clearFilters();

	delete[] middleFreqs;
	middleFreqs = nullptr;

	clearCompressor();
}

void saola::channelSplit_wShelf::init(double sampleRate, int numSamples, int numSplits, int numChannels)
{
	clearChannels();
	clearCompressor();

	numOfSamples = numSamples;
	numOfSplits = numSplits;
	numOfChannels = numChannels;
	sampleOfRates = sampleRate;

	middleFreqs = new double[numOfSplits];
	gainReduction = new float[numOfSplits];

	channelArray = new float** [numOfSplits];
	compressArray = new compress[numOfSplits];

	for (int i = 0; i < numOfSplits; i++)
	{
		gainReduction[i] = 0;

		channelArray[i] = new float* [2];
		channelArray[i][0] = new float[numOfSamples];
		channelArray[i][1] = new float[numOfSamples];

		compressArray[i].init(numOfSamples, sampleOfRates, 0.05, numOfChannels);
	}

	//if (numOfSplits > 1)
	freqSplitSetup();
}

void saola::channelSplit_wShelf::freqSplitSetup()
{
	clearFilters();

	filterBP = new Dsp::SimpleFilter <Dsp::Butterworth::BandPass<2>, 2>**[numOfSplits - 2.];

	/* linear
	double cutWidths = (20000 - 20) / (numOfSplits + 1);
	double freqcutTMP = cutWidths + 20;
	*/

	double cutWidths = (4.301 - 1.301) / numOfSplits;
	double freqcutTMP = 1.301 + cutWidths;

	for (int i = 0; i < numOfSplits; i++)
	{
		if (i == 0)
		{
			filterLP = new Dsp::SimpleFilter <Dsp::Butterworth::LowPass<4>, 2> *[2];
			filterLP[0] = new Dsp::SimpleFilter <Dsp::Butterworth::LowPass<4>, 2>;
			filterLP[1] = new Dsp::SimpleFilter <Dsp::Butterworth::LowPass<4>, 2>;
			filterLP[0]->setup(4, sampleOfRates, pow(10, freqcutTMP));
			filterLP[1]->setup(4, sampleOfRates, pow(10, freqcutTMP));
			middleFreqs[i] = pow(10, freqcutTMP);
		}
		else if (i == (numOfSplits - 1))
		{
			filterHP = new Dsp::SimpleFilter <Dsp::Butterworth::HighPass<4>, 2> *[2];
			filterHP[0] = new Dsp::SimpleFilter <Dsp::Butterworth::HighPass<4>, 2>;
			filterHP[1] = new Dsp::SimpleFilter <Dsp::Butterworth::HighPass<4>, 2>;
			filterHP[0]->setup(4, sampleOfRates, pow(10, freqcutTMP));
			filterHP[1]->setup(4, sampleOfRates, pow(10, freqcutTMP));
			middleFreqs[i] = pow(10, freqcutTMP);
		}
		else
		{
			filterBP[i] = new Dsp::SimpleFilter <Dsp::Butterworth::BandPass<2>, 2> *[2];
			filterBP[i][0] = new Dsp::SimpleFilter <Dsp::Butterworth::BandPass<2>, 2>;
			filterBP[i][1] = new Dsp::SimpleFilter <Dsp::Butterworth::BandPass<2>, 2>;

			double width = pow(10, freqcutTMP + cutWidths) - pow(10, freqcutTMP);
			double center = pow(10, freqcutTMP) + width / 2;

			
			middleFreqs[i] = pow(10, freqcutTMP);
			freqcutTMP += cutWidths;

			filterBP[i][0]->setup(2, sampleOfRates, center, width);
			filterBP[i][1]->setup(2, sampleOfRates, center, width);
			
		}
	}

	filterBS = new Dsp::SimpleFilter <Dsp::Butterworth::BandShelf<4>, 2>[numOfSplits];
}

void saola::channelSplit_wShelf::shelfFilterSetup(float** rawInput)
{

	for (int i = 0; i < numOfSplits; i++)
	{
		if (i == 0)
		{
			filterLS.setup(4, sampleOfRates, middleFreqs[i], gainReduction[i]);
			filterLS.process(numOfSamples, rawInput);

		}
		else if(i == numOfSplits-1)
		{
			filterHS.setup(4, sampleOfRates, middleFreqs[i-1], gainReduction[i]);
			filterHS.process(numOfSamples, rawInput);

		}
		else
		{
			filterBS[i].setup(4, sampleOfRates, (middleFreqs[i] + middleFreqs[i + 1]) / 2.f, middleFreqs[i + 1] - middleFreqs[i], gainReduction[i]);
			filterBS[i].process(numOfSamples, rawInput);
		}
	}
}

float* saola::channelSplit_wShelf::calcGainReduction()
{
	for (int i = 0; i < numOfSplits; i++)
	{
		//gainReduction = compressArray[i].compProcess(channelArray[i], numOfSamples);

		gainReduction[i] = -12;
	}
	return gainReduction;
}



float*** saola::channelSplit_wShelf::multiplyAndFilter(float** in, int numSamples)
{
	numOfSamples = numSamples;

	for (int i = 0; i < numOfSplits; i++)
	{
		if (sizeof(in[0]) == sizeof(channelArray[i][0]))
		{
			std::memcpy(channelArray[i][0], in[0], numSamples * sizeof(float));
			std::memcpy(channelArray[i][1], in[1], numSamples * sizeof(float));
		}
	}
	freqOut();

	return channelArray;
}

void saola::channelSplit_wShelf::freqOut()
{

	for (int i = 0; i < numOfSplits; i++)
	{
		if (i == 0)
		{
			filterLP[0]->process(numOfSamples, channelArray[i]);
			filterLP[1]->process(numOfSamples, channelArray[i]);
		}
		else if (i == (numOfSplits - 1))
		{
			filterHP[0]->process(numOfSamples, channelArray[i]);
			filterHP[1]->process(numOfSamples, channelArray[i]);
		}
		else
		{
			filterBP[i][0]->process(numOfSamples, channelArray[i]);
			filterBP[i][1]->process(numOfSamples, channelArray[i]);
		}
	}

}

void saola::channelSplit_wShelf::compressorSetup(double attack_ms, double release_ms, double ratio, double* threshold_dB, int type)
{
	for (int i = 0; i < numOfSplits; i++)
	{
		compressArray[i].setup(attack_ms, release_ms, ratio, threshold_dB[i], type);
	}
	mThreshold = pow(10, threshold_dB[0] / 20); //only if the threshold is the same everywhere
}

void saola::channelSplit_wShelf::compressAndSumUp(float** out)
{
	for (int i = 0; i < numOfChannels; i++)
	{
		memset(out[i], 0.0f, numOfSamples * sizeof(float));
	}

	for (int i = 0; i < numOfSplits; i++)
	{
		compressArray[i].compProcess(channelArray[i], numOfSamples);

		for (int j = 0; j < numOfChannels; j++)
		{

			int samples = numOfSamples;

			float* ptrIn = channelArray[i][j];
			float* ptrsum = out[j];

			while (--samples >= 0)
			{
				if (i == 0 || i == numOfSplits - 1)
					(*ptrsum++) -= (*ptrIn++);
				else
					(*ptrsum++) += (*ptrIn++);
			}
		}
	}
}

void saola::channelSplit_wShelf::justSumUp(float** in)
{

	for (int i = 0; i < numOfChannels; i++)
	{
		memset(in[i], 0.0f, numOfSamples * sizeof(float));
	}

	for (int i = 0; i < numOfSplits; i++)
	{
		for (int j = 0; j < numOfChannels; j++)
		{

			int samples = numOfSamples;

			float* ptrIn = channelArray[i][j];
			float* ptrsum = in[j];

			while (--samples >= 0)
			{
				if (i == 0 || i == numOfSplits - 1)
					(*ptrsum++) -= (*ptrIn++);
				else
					(*ptrsum++) += (*ptrIn++);
			}
		}
	}
}

void saola::channelSplit_wShelf::dryWetAdjust(float** in, float** out, float amount)
{

	for (int i = 0; i < numOfChannels; i++)
	{

		int samples = numOfSamples;

		float* ptrIn = in[i];
		float* ptrOut = out[i];

		while (--samples >= 0)
		{
			if (isDeltaEnabled)
			{
				*ptrOut -= (*ptrIn * mThreshold);
			}

			*ptrIn *= (1 - amount);
			*ptrOut *= amount;
			*ptrOut++ += *ptrIn++;
		}
	}


}

void saola::channelSplit_wShelf::clearFilters()
{
	if (filterBP != nullptr)
	{
		for (int i = 0; i < numOfSplits; i++)
		{
			if (i == 0)
			{
				delete filterLP[0];
				delete filterLP[1];
				delete[] filterLP;
				filterLP = nullptr;
			}
			else if (i == (numOfSplits - 1))
			{

				delete filterHP[0];
				delete filterHP[1];
				delete[] filterHP;
				filterHP = nullptr;
			}
			else
			{

				delete filterBP[i][0];
				delete filterBP[i][1];
				delete[] filterBP[i];
				
			}

		}
		delete[] filterBP;
		filterBP = nullptr;
		delete[] filterBS;
		filterBS = nullptr;
	}

}

void saola::channelSplit_wShelf::clearChannels()
{

	if (channelArray != nullptr)
	{

		for (int i = 0; i < numOfSplits; i++)
		{

			delete[] channelArray[i][0];
			delete[] channelArray[i][1];
			delete[] channelArray[i];
		}
		delete[] channelArray;
		channelArray = nullptr;
	}
}

void saola::channelSplit_wShelf::clearCompressor()
{
	if (compressArray != nullptr)
	{
		delete[] compressArray;
		compressArray = nullptr;
	}
}

void saola::channelSplit_wShelf::compensate(float** out, double howMuchdB, bool forInsideChannelsOnly)
{
	float ratiocomp = powf(10.f, howMuchdB / 20.f);

	if (forInsideChannelsOnly)
	{
		for (int i = 0; i < numOfSplits; i++)
		{
			for (int j = 0; j < numOfChannels; j++)
			{
				int samples = numOfSamples;
				float* ptrIn = channelArray[i][j];
				while (--samples >= 0)
				{
					(*ptrIn) *= ratiocomp;
					ptrIn++;
				}
			}
		}
	}
	else
	{
		for (int j = 0; j < numOfChannels; j++)
		{
			int samples = numOfSamples;
			float* ptr = out[j];
			while (--samples >= 0)
				(*ptr++) *= ratiocomp;
		}
	}

}

void saola::channelSplit_wShelf::isDelta(bool Enabled)
{
	isDeltaEnabled = Enabled;
}



