/*
  ==============================================================================

    ThresholdEditor.h
    Created: 2 Nov 2021 7:19:12pm
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

class ThresholdEditor : public juce::Component//, juce::AudioProcessorEditor
{
public:
    ThresholdEditor(DynamifyIIAudioProcessor&);
    ~ThresholdEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void getNumOfBands(int numBands) { mNumBands = numBands; };

private:
    juce::Slider* mThresholdSlider = nullptr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>* thresholdAttach = nullptr;

    int mNumBands = NUMBANDS;

    DynamifyIIAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThresholdEditor)
};