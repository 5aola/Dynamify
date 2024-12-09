#include "compressor.h"
#include <cmath>

saola::compress::compress()
{
	mNumChannels = 2;

	VU = nullptr;
	attackTime = 1;
	releaseTime = 1;
	ratio = 1;
	thresdB = 1;
	sRate = 44100;

	mGain = nullptr;
	mType = tCompress;
}

saola::compress::~compress()
{
	delete VU;
	VU = nullptr;

	delete[] mGain;
	mGain = nullptr;
}

void saola::compress::init(int numSamples, double sampleRate, double tau, int numChannels)
{
	VU = new VUmeter;
	VU->set(sampleRate, numSamples, tau);
	sRate = sampleRate;
	mNumChannels = numChannels;

	gainReduction = new float[numSamples];
	memset(gainReduction, 0, sizeof(float) * numSamples);

	mGain = new float[mNumChannels];
	for (int i = 0; i < mNumChannels; i++)
		mGain[i] = 0;
}

void saola::compress::setup(double attackTimeMS, double releaseTimeMS, double kratio, double thresHolddB, int type)
{
	
	ratio = kratio;
	thresdB = thresHolddB;
	
	attackTime = 1 / (attackTimeMS * 0.001 * sRate);
	releaseTime = 1 / (releaseTimeMS * 0.001 * sRate);

	if (ratio > 63.99 && type == tCompress)
		mType = tLimit;
	else
		mType = type;
}

float* saola::compress::compProcess(float** in, int numSamples)
{


	float** ptrRMS = VU->calculateRMSIndB(in, numSamples);

	for (int i = 0; i < mNumChannels; i++)
	{
		float* tmpRMS = ptrRMS[i];
		float* tmpIn = in[i];

		float* gainr = gainReduction;
		int samples = numSamples;
		while (--samples >= 0)
		{
			float tmpDelta = (thresdB - *tmpRMS);
			float mLocalGainIndB;
			switch (mType)
			{
			case tCompress:
				if (*tmpRMS > thresdB)
					mLocalGainIndB = (1 - 1 / ratio) * tmpDelta;
				else
					mLocalGainIndB = 0;
				break;
			case tExpand:
				if (*tmpRMS < thresdB)
					mLocalGainIndB = (1 - 1 / ratio) * (-tmpDelta);
				else
					mLocalGainIndB = 0;
				break;
			case tUpwardCompress:
				if (tmpDelta > 64)
					tmpDelta = 64;
				if (*tmpRMS < thresdB)
					mLocalGainIndB = (1 - 1 / ratio) * tmpDelta;
				else
					mLocalGainIndB = 0;
				break;
			case tUpDownCompress:
				if (tmpDelta > 64)
					tmpDelta = 64;

				mLocalGainIndB = (1 - 1 / ratio) * tmpDelta;
				break;
			case tLimit:
				if (*tmpRMS > thresdB)
					mLocalGainIndB = tmpDelta;
				else
					mLocalGainIndB = 0;
				break;
			case tGate:
				if (*tmpRMS < thresdB)
					mLocalGainIndB = -64;
				else
					mLocalGainIndB = 0;
				break;
			default:
				mLocalGainIndB = 0;
				break;
			}

			mLocalGainIndB = pow(10, mLocalGainIndB / 20); //converting to gainreduction, not in dB anymore


			double coeff;
			if (mLocalGainIndB < mGain[i])
				coeff = attackTime;
			else
				coeff = releaseTime;

			mGain[i] = (1 - coeff) * mGain[i] + coeff * mLocalGainIndB;
			*tmpIn *= mGain[i];

			if (i == 0)
			{
				*gainr = 20 * log10(sqrt(mGain[i]));
				gainr++;
			}
			
			tmpIn++;
			tmpRMS++;
		}
	}
	return gainReduction;
}

