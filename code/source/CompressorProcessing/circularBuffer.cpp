#include "circularBuffer.h"

saola::circularBuffer::circularBuffer()
{
	mBuffer = nullptr;
	mOutputBuffer[0] = nullptr;
	mOutputBuffer[1] = nullptr;
	mNumChannels = 0;
	mSize = 0;
	mMultiply = 0;

	mWritePosition = 0;
}

saola::circularBuffer::~circularBuffer()
{
	reset();
}

void saola::circularBuffer::setSize(int numChannels, int size, int multiply)
{
	mNumChannels = numChannels;
	mSize = size;
	mMultiply = multiply;

	reset();

	mBuffer = new float* [mNumChannels];
	mOutputBuffer[0] = new float*[mNumChannels];
	mOutputBuffer[1] = new float* [mNumChannels];
	for (int i = 0; i < mNumChannels; i++)
	{
		mBuffer[i] = new float[mSize * mMultiply];
		mOutputBuffer[0][i] = new float[mSize];
		mOutputBuffer[1][i] = new float[mSize];
	}

	clearToZeros();
}

void saola::circularBuffer::addBuffer(float** inputBuffer, int numSamples)
{
	//mMutex.lock();
	for (int channel = 0; channel < mNumChannels; channel++)
	{
		int tempBufferPos = mWritePosition;
		for (int sample = 0; sample < numSamples; sample++)
		{
			mBuffer[channel][tempBufferPos] = inputBuffer[channel][sample];

			tempBufferPos++;
			tempBufferPos = tempBufferPos % (mSize * mMultiply);
		}
	}
	mWritePosition = (mWritePosition + numSamples) % (mSize * mMultiply);
	updateOutputBuffer();
	//mMutex.unlock();
}

float** saola::circularBuffer::getBuffer()
{
	return mOutputBuffer[mValidOutput];
}

void saola::circularBuffer::updateOutputBuffer()
{
	//mMutex.lock();
	int tmpOut = mValidOutput + 1;
	tmpOut = tmpOut % 2;

	for (int channel = 0; channel < mNumChannels; channel++)
	{
		for (int sample = 0; sample < mSize; sample++)
		{
			int tmpPos = mWritePosition - mSize + sample;
			if (tmpPos < 0)
				tmpPos += (mSize * mMultiply);
			tmpPos = tmpPos % (mSize * mMultiply);
			//DBG(tmpPos);
			mOutputBuffer[tmpOut][channel][sample] = mBuffer[channel][tmpPos];
		}
	}
	mValidOutput++;
	mValidOutput = mValidOutput % 2;
	//mMutex.unlock();
}

void saola::circularBuffer::clearToZeros()
{
	for (int i = 0; i < mNumChannels; i++)
	{
		memset(mBuffer[i], 0.0f, mSize * mMultiply * sizeof(float));
		memset(mOutputBuffer[0][i], 0.0f, mSize * sizeof(float));
		memset(mOutputBuffer[1][i], 0.0f, mSize * sizeof(float));
	}	
}

void saola::circularBuffer::reset()
{
	if (mBuffer != nullptr)
	{
		for (int i = 0; i < mNumChannels; i++)
			delete[] mBuffer[i];
		delete[] mBuffer;
		mBuffer = nullptr;
	}
	if (mOutputBuffer[0] != nullptr)
	{
		for (int i = 0; i < mNumChannels; i++)
		{
			delete[] mOutputBuffer[0][i];
			delete[] mOutputBuffer[1][i];
		}
			
		delete[] mOutputBuffer[0];
		delete[] mOutputBuffer[1];
		mOutputBuffer[0] = nullptr;
		mOutputBuffer[1] = nullptr;
	}
}
