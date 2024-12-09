/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrumView.h"
#include "ThresholdEditor.h"
#include "CompressorProcessing/FFTCalculator.h"

//==============================================================================
/**
*/
class DynamifyIIAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DynamifyIIAudioProcessorEditor (DynamifyIIAudioProcessor&/*, saola::FFTCalculator&*/);
    ~DynamifyIIAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::Rectangle<int> rect;

    juce::Slider mAttackSlider;
    juce::Slider mReleaseSlider;
    juce::Slider mRatioSlider;
    juce::Slider mInputGainSlider;
    juce::Slider mOutputGainSlider;
    juce::Slider mMixSlider;

    juce::ComboBox compressorMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> MenuAttachment;

    SpectrumView mSpectrumView;
    ThresholdEditor mThresholdEditor;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach;

    juce::Label texts[6];



    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DynamifyIIAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynamifyIIAudioProcessorEditor)
};
