/*
  ==============================================================================

    SpectrumView.h
    Created: 24 Oct 2021 4:00:47pm
    Author:  kosty

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CompressorProcessing/FFTCalculator.h"

#ifndef PI
#define PI  3.1415926535897932384626433832795
#endif

//==============================================================================

class SpectrumView : public juce::Component, public juce::Timer
{
public:
    SpectrumView(DynamifyIIAudioProcessor&);
    ~SpectrumView() override;

    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setResolution(int size);

    void calcMaxAmp();

    void resetBuffers();

private:
    int mSize = 2048;
    int mVisibleSize = 0;
    float* mBuffer[2];
    float* mfreqPositions;
    //saola::FFTCalculator* mFFTCalc;

    juce::DrawableText mTestText;


    int count = 0;
    float mMaxAmp[2];

    int mCntrUnder20Hz = 0, mCntrAbove20kHz = 0;

    juce::Image mBackground;

    DynamifyIIAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumView)

};
