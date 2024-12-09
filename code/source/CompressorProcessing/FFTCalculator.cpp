/*
  ==============================================================================

    FFTCalculator.cpp
    Created: 21 Oct 2021 8:16:09pm
    Author:  kosty

  ==============================================================================
*/

#include "FFTCalculator.h"

saola::FFTCalculator::FFTCalculator()
{
    mCircularBuffer = nullptr;
    fftin = nullptr;
    fftout = nullptr;
    mOutputBuffer[0] = nullptr;
    mOutputBuffer[1] = nullptr;
    mNumChannels = 0;
    mSampleRate = 0;
    mSize = 4096;
    p = nullptr;
}

saola::FFTCalculator::~FFTCalculator()
{
    reset();
}

void saola::FFTCalculator::init(int bufferSize, int numChannels, double sampleRate)
{
    mNumChannels = numChannels;
    mSize = bufferSize;
    mSampleRate = sampleRate;

    //reset();

    mCircularBuffer = new circularBuffer;
    mCircularBuffer->setSize(mNumChannels, mSize, 2);

    mOutputBuffer[0] = new float[mSize/2];
    mOutputBuffer[1] = new float[mSize/2];

    memset(mOutputBuffer[0], -90.0f, sizeof(float) * mSize/2);
    memset(mOutputBuffer[1], -90.0f, sizeof(float) * mSize/2);

    mFreqs = new float[mSize / 2];
    memset(mFreqs, 0.0f, sizeof(float) * mSize / 2);


    fftin = (double*)fftw_malloc(sizeof(double) * mSize);
    fftout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * mSize);
    memset(fftout, -120, sizeof(fftw_complex) * mSize);
    p = fftw_plan_dft_r2c_1d(mSize, fftin, fftout, FFTW_ESTIMATE);
}

float* saola::FFTCalculator::getBuffer()
{
    float** tmpBuffer = mCircularBuffer->getBuffer();
    makeItMono(tmpBuffer);
    applyHanningWindow(fftin);
    fftw_execute(p);
    float* out = calculateAmplitudeOfComplexArray(fftout);
    setSpectrumSpeed();
    
    /*for (size_t i = 0; i < mSize; i++)
        out[i] = (float)fftin[i];*/

    return mOutputBuffer[1]; 
}

float* saola::FFTCalculator::getFreqs()
{
    for (int i = 0; i < mSize / 2; i++)
        mFreqs[i] = (i * mSampleRate) / mSize; // calculate the frequencies

    return mFreqs;
}

void saola::FFTCalculator::applyHanningWindow(double* buffer)
{
    for (int sample = 0; sample < mSize; sample++)
        buffer[sample] = buffer[sample] * 0.5 * (1.0 - cos(2.0 * PI * sample / (mSize - 1)));
}

void saola::FFTCalculator::setSpectrumSpeed(float ReleaseTime)
{
    float coeffrel = 1.f / ((float)ReleaseTime * (float)FPS);
    float coeffatt = 1.f / ((float)ReleaseTime * (float)FPS / 20.f);

    for (auto i = 0; i < mSize / 2; ++i)
    {
        if (mOutputBuffer[0][i] < -120)
            mOutputBuffer[0][i] = -120;

        if (mOutputBuffer[0][i] < mOutputBuffer[1][i])
            mOutputBuffer[1][i] = ((float)1 - coeffrel) * mOutputBuffer[1][i] + coeffrel * mOutputBuffer[0][i];
        else
            mOutputBuffer[1][i] = ((float)1 - coeffatt) * mOutputBuffer[1][i] + coeffatt * mOutputBuffer[0][i];
    }
}

void saola::FFTCalculator::makeItMono(float** buffer) //gets a stereo buffer and copy it to fftin
{
    for (int sample = 0; sample < mSize; sample++)
    {
        for (int channel = 0; channel < mNumChannels; channel++) // adding the elements to the 0 channel
        {
            if (channel == 0)
                fftin[sample] = (double)buffer[channel][sample]; // saving in the fftin buffer
            else
                fftin[sample] += (double)buffer[channel][sample];
        }
        fftin[sample] /= (double)mNumChannels;
    }
}

float* saola::FFTCalculator::calculateAmplitudeOfComplexArray(fftw_complex* complexArray)
{
    for (int i = 0; i < mSize / 2; i++)
    {
        mOutputBuffer[0][i] = (float)sqrt(complexArray[i][0] * complexArray[i][0] + complexArray[i][1] * complexArray[i][1]); // calculate the amplitudes
        mOutputBuffer[0][i] /= (mSize / 4);                                     // compesnating amplitudes
        mOutputBuffer[0][i] = 20 * log10(mOutputBuffer[0][i]); // calculate amplitude in dB
    }
    return mOutputBuffer[0];
}

void saola::FFTCalculator::reset()
{
    fftw_destroy_plan(p);
    fftw_free(fftin);
    fftw_free(fftout);

    if (mCircularBuffer != nullptr)
    {
        delete mCircularBuffer;
        mCircularBuffer = nullptr;
    }

    if (mFreqs != nullptr)
    {
        delete[] mFreqs;
        mFreqs = nullptr;
    }

    if (mOutputBuffer[0] != nullptr)
    {
        delete[] mOutputBuffer[0];
        mOutputBuffer[0] = nullptr;
        //mOutputBuffer[1] = nullptr;
    }
    if (mOutputBuffer[1] != nullptr)
    {
        delete[] mOutputBuffer[1];
        //mOutputBuffer[0] = nullptr;
        mOutputBuffer[1] = nullptr;
    }
}
