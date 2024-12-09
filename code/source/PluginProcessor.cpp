/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::NormalisableRange<float> frequencyRange(float min, float max, float interval)
{
    return { min, max, interval, 1.f / std::log2(1.f + std::sqrt(max / min)) };
}
//==============================================================================
DynamifyIIAudioProcessor::DynamifyIIAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    tree(*this, nullptr, juce::Identifier("PARAMETERS"), createParameters())
#endif
{
    mFTTCalculator[0] = new saola::FFTCalculator;
    mFTTCalculator[1] = new saola::FFTCalculator;
    tree.state.addListener(this);
}

DynamifyIIAudioProcessor::~DynamifyIIAudioProcessor()
{
    delete mFTTCalculator[0];
    delete mFTTCalculator[1];
}

//==============================================================================
const juce::String DynamifyIIAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DynamifyIIAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DynamifyIIAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DynamifyIIAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DynamifyIIAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DynamifyIIAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DynamifyIIAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DynamifyIIAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DynamifyIIAudioProcessor::getProgramName (int index)
{
    return {};
}

void DynamifyIIAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DynamifyIIAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int mNumBands = NUMBANDS;

    mMultibandProcessing = new saola::channelSplit;
    mMultibandProcessing->init(sampleRate, samplesPerBlock, mNumBands);

    
    //mMultibandShelfProcessing = new saola::channelSplit_wShelf;
    //mMultibandShelfProcessing->init(sampleRate, samplesPerBlock, mNumBands);

    arrayOfThresholdValues = new double[mNumBands];
    for (size_t i = 0; i < mNumBands; i++)
        arrayOfThresholdValues[i] = 0;

    mDryChannel = new float* [2];
    for (size_t i = 0; i < 2; i++)
        mDryChannel[i] = new float[samplesPerBlock];


    /*fftin = (double*)fftw_malloc(sizeof(double) * samplesPerBlock);
    fftout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerBlock);
    p = fftw_plan_dft_r2c_1d(samplesPerBlock, fftin, fftout, FFTW_ESTIMATE);*/

    mFTTCalculator[0]->init(BUFFERSIZE, 2, sampleRate);
    mFTTCalculator[1]->init(BUFFERSIZE, 2, sampleRate);
}

void DynamifyIIAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.


    /*delete mMultibandProcessing;
    delete[] arrayOfThresholdValues;
    for (size_t i = 0; i < 2; i++)
        delete[] mDryChannel[i];
    delete[] mDryChannel;*/
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DynamifyIIAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DynamifyIIAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    attackParam = tree.getRawParameterValue("ATTACK")->load();
    releaseParam = tree.getRawParameterValue("RELEASE")->load();
    ratioParam = tree.getRawParameterValue("RATIO")->load();
    inputGainParam = tree.getRawParameterValue("INPUTGAIN")->load();
    outputGainParam = tree.getRawParameterValue("OUTPUTGAIN")->load();
    mixParam = tree.getRawParameterValue("MIX")->load();
    mixParam /= 100;
    typeParam = tree.getRawParameterValue("TYPE")->load();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto audioBuffer = buffer.getArrayOfWritePointers();

    

    // CompressorProcessing:

    mMultibandProcessing->multiplyAndFilter(audioBuffer, buffer.getNumSamples());

    mMultibandProcessing->justSumUp(mDryChannel);

    mMultibandProcessing->compensate(mDryChannel, inputGainParam, true);

    for (size_t i = 0; i < mMultibandProcessing->getNumOfSplits(); i++)
        arrayOfThresholdValues[i] = tree.getRawParameterValue(juce::String(i))->load();
    mMultibandProcessing->compressorSetup(attackParam, releaseParam, ratioParam, arrayOfThresholdValues, typeParam);

    mMultibandProcessing->compressAndSumUp(audioBuffer);

    mMultibandProcessing->compensate(audioBuffer, outputGainParam);

    mFTTCalculator[0]->addBuffer(mDryChannel, buffer.getNumSamples());
    
    mMultibandProcessing->dryWetAdjust(mDryChannel, audioBuffer, mixParam);

    mFTTCalculator[1]->addBuffer(audioBuffer, buffer.getNumSamples());

    
}

//==============================================================================
bool DynamifyIIAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DynamifyIIAudioProcessor::createEditor()
{
    return new DynamifyIIAudioProcessorEditor (*this);
}

//==============================================================================
void DynamifyIIAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DynamifyIIAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void DynamifyIIAudioProcessor::valueTreePropertyChanged(juce::ValueTree& threeWhosePropertyHasChanged, const juce::Identifier& property)
{
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DynamifyIIAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout DynamifyIIAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "attack", frequencyRange(0.001f, 1000.f, 0.00001f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "release", frequencyRange(0.1f, 2000.f, 0.00001f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RATIO", "raito", frequencyRange(1.f, 64.f, 0.01f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("INPUTGAIN", "inputgain", -32.0f, 32.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUTGAIN", "outputgain", -32.0f, 32.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "mix", 0.0f, 100.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TYPE", "type", 1.f, 6.0f, 1.0f));

    for (size_t i = 0; i < NUMBANDS; i++)
    {
        auto name = juce::String(i);
        params.push_back(std::make_unique<juce::AudioParameterFloat>(name, name, -90.f, 0.0f, 0.0f));
    }

    return { params.begin(), params.end() };
}
