/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CompressorProcessing/channelSplit.h"
#include "CompressorProcessing/FFTCalculator.h"
#include "CompressorProcessing/channelSplit_wShelf.h"
#include "fftw3.h"

#define TWOPI 6.283185307179586476925286766559

//==============================================================================
/**
*/
class DynamifyIIAudioProcessor  : public juce::AudioProcessor, public juce::ValueTree::Listener
{
public:
    //==============================================================================
    DynamifyIIAudioProcessor();
    ~DynamifyIIAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState tree;
    saola::FFTCalculator* mFTTCalculator[2];

private:

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void valueTreePropertyChanged(juce::ValueTree& threeWhosePropertyHasChanged, const juce::Identifier& property) override;
    std::atomic<bool> ShouldUpdate{ false };

    saola::channelSplit* mMultibandProcessing;
    saola::channelSplit_wShelf* mMultibandShelfProcessing;

    float attackParam = 1;
    float releaseParam = 1;
    float ratioParam = 4;
    float inputGainParam = 0;
    float outputGainParam = 0;
    float mixParam = 1;
    float typeParam = 1;

    double* arrayOfThresholdValues = nullptr;;

    float** mDryChannel = nullptr;

    
    Dsp::SimpleFilter <Dsp::Butterworth::BandShelf<4>, 2> filterBS;
    float gainBS = 0;
    float phase = 0;
    double* fftin;
    fftw_complex* fftout;
    fftw_plan p;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynamifyIIAudioProcessor)
};
