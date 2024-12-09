#pragma once

#include "base/source/fstreamer.h"
#include "compressor.h"
#include "paramids.h"
#include <JuceHeader.h>

namespace saola {

	class circularBuffer
	{
	public:
		circularBuffer();
		~circularBuffer();
		void setSize(int numChannels, int size, int multiply);

		void addBuffer(float** inputBuffer, int numSamples);
		float** getBuffer();
		
		int getNumChannels() { return mNumChannels; };
		int getSize() { return mSize; };

	protected:
		void clearToZeros();
		void reset();

		void updateOutputBuffer();

		float** mBuffer = nullptr;
		float** mOutputBuffer[2];

		int mValidOutput = 0;

		std::mutex mMutex;

		int mWritePosition = 0;

		int mNumChannels, mSize, mMultiply;
	};

}