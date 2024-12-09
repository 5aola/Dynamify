/*
  ==============================================================================

    ThresholdEditor.cpp
    Created: 2 Nov 2021 7:19:12pm
    Author:  kosty

  ==============================================================================
*/

#include "ThresholdEditor.h"

ThresholdEditor::ThresholdEditor(DynamifyIIAudioProcessor& p) : audioProcessor(p)
{
    mThresholdSlider = new juce::Slider[mNumBands];
    thresholdAttach = new std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>[mNumBands];
    for (int i = 0; i < mNumBands; i++)
    {
        mThresholdSlider[i].setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        mThresholdSlider[i].setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0.0, 0.0);
        mThresholdSlider[i].setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
        mThresholdSlider[i].setColour(juce::Slider::ColourIds::trackColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
        mThresholdSlider[i].setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(0xfff9b132));
        addAndMakeVisible(&mThresholdSlider[i]);

        auto name = juce::String(i);
        thresholdAttach[i].reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, name, mThresholdSlider[i]));
    }
}

ThresholdEditor::~ThresholdEditor()
{

}

void ThresholdEditor::paint(juce::Graphics& g)
{
    //g.fillAll(juce::Colour(0xFF595959)); //darkgrey

    g.setColour(juce::Colour(0xffb3b3b3));

    g.setOpacity(0.0);
}

void ThresholdEditor::resized()
{
    auto area = getLocalBounds();
    auto width = (float)area.getWidth();
    for (int i = 0; i < mNumBands; i++)
    {
        mThresholdSlider[i].setBounds(area.removeFromLeft(width / (float)mNumBands));
    }
    
}
