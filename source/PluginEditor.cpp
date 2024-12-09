/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DynamifyIIAudioProcessorEditor::DynamifyIIAudioProcessorEditor(DynamifyIIAudioProcessor& p/*, saola::FFTCalculator& fft*/)
    : AudioProcessorEditor(&p), audioProcessor(p), mSpectrumView(p), mThresholdEditor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    auto size = 200;
    setSize(4 * size, 3 * size);

    compressorMenu.addItem("Compressor", 1);
    compressorMenu.addItem("Upwards Compressor", 2);
    compressorMenu.addItem("All Direction Compressor", 3);
    //compressorMenu.addItem("Upwards Expander", 4);
    compressorMenu.addItem("Limiter", 4);
    compressorMenu.addItem("Expander", 5);
    compressorMenu.addItem("Gate", 6);
    compressorMenu.setJustificationType(juce::Justification::centred);
    compressorMenu.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(0xffb3b3b3));
    compressorMenu.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colour(0xffb3b3b3));
    compressorMenu.setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colour(0xff2d2d2d));
    compressorMenu.setColour(juce::ComboBox::ColourIds::textColourId, juce::Colour(0xff2d2d2d));
    compressorMenu.setColour(juce::ComboBox::ColourIds::buttonColourId, juce::Colour(0xffb3b3b3));
    addAndMakeVisible(&compressorMenu);

    MenuAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.tree, "TYPE", compressorMenu));

    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mAttackSlider.setTextValueSuffix(" ms");
    mAttackSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mAttackSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mAttackSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour((uint8)0, 0, 0, 0.f)); // f9b132
    mAttackSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xfff39420)); //f39420
    addAndMakeVisible(&mAttackSlider);

    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mReleaseSlider.setTextValueSuffix(" ms");
    mReleaseSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mReleaseSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mReleaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour((uint8)0, 0, 0, 0.f));
    mReleaseSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xfff39420));
    addAndMakeVisible(&mReleaseSlider);

    mRatioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRatioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mRatioSlider.setTextValueSuffix(" : 1");
    mRatioSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mRatioSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mRatioSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour((uint8)0, 0, 0, 0.f));
    mRatioSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xfff39420));
    addAndMakeVisible(&mRatioSlider);

    mInputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mInputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mInputGainSlider.setTextValueSuffix(" dB");
    mInputGainSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mInputGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mInputGainSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(0xfff9b132));
    mInputGainSlider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colour(0xfff39420));
    addAndMakeVisible(&mInputGainSlider);

    mOutputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mOutputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mOutputGainSlider.setTextValueSuffix(" dB");
    mOutputGainSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mOutputGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mOutputGainSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(0xfff9b132));
    mOutputGainSlider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colour::Colour(0xfff39420));
    mOutputGainSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xfff39420));
    addAndMakeVisible(&mOutputGainSlider);

    mMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100.0, 20.0);
    mMixSlider.setTextValueSuffix(" %");
    mMixSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, juce::Colour(0xff2d2d2d));
    mMixSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colour::Colour((uint8)0, 0, 0, 0.f));
    mMixSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour((uint8)0, 0, 0, 0.f));
    mMixSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(0xfff39420));
    addAndMakeVisible(&mMixSlider);

    addAndMakeVisible(&mSpectrumView);
    addAndMakeVisible(&mThresholdEditor);
    //mSpectrumView.setResolution(audioProcessor.mFTTCalculator->getSize());

    attackAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "ATTACK", mAttackSlider));
    releaseAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "RELEASE", mReleaseSlider));
    ratioAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "RATIO", mRatioSlider));
    inputGainAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "INPUTGAIN", mInputGainSlider));
    outputGainAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "OUTPUTGAIN", mOutputGainSlider));
    mixAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "MIX", mMixSlider));




    // Adding Texts =====================================================================

    for (size_t i = 0; i < 6; i++)
    {
        addAndMakeVisible(texts[i]);
        texts[i].setFont(juce::Font(16.0f));
        texts[i].setColour(juce::Label::textColourId, juce::Colour(0xff2d2d2d));
        texts[i].setJustificationType(juce::Justification::centred);
    }
    texts[0].setText("Attack Time", juce::dontSendNotification);
    texts[1].setText("Release Time", juce::dontSendNotification);
    texts[2].setText("Ratio", juce::dontSendNotification);
    texts[3].setText("Input Gain", juce::dontSendNotification);
    texts[4].setText("Output Gain", juce::dontSendNotification);
    texts[5].setText("Mix", juce::dontSendNotification);
}

DynamifyIIAudioProcessorEditor::~DynamifyIIAudioProcessorEditor()
{
}

//==============================================================================
void DynamifyIIAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(0xFF3c3c3c)); //darkgrey

    g.setColour(juce::Colour(0xffb3b3b3));

    g.setFont(15.0f);

    auto area = getLocalBounds();
    area.reduce(6, 6);

    const juce::Rectangle<float> areaFix = area.toFloat();
    g.fillRoundedRectangle(areaFix, 10.0f);

    g.drawText("Oscillators", 23, 10, 200, 20, juce::Justification::centredLeft);
    //juce::Rectangle<int> area(10, 10, 40, 40);

}

void DynamifyIIAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(10, 10);
    auto topArea = area.removeFromTop(area.getHeight() * 2 / 3);
    auto ComboBoxArea = topArea.removeFromTop(area.getHeight() / 8);
    auto topSliderAreaWidth = topArea.getWidth() / 10;
    auto topSliderArea = topArea;
    topSliderArea.removeFromTop(20);
    topSliderArea.removeFromBottom(20);

    auto leftSliderArea = topSliderArea.removeFromLeft(topSliderAreaWidth);
    auto rightSliderArea = topSliderArea.removeFromRight(topSliderAreaWidth);
    texts[3].setBounds(leftSliderArea.removeFromTop(20));
    texts[4].setBounds(rightSliderArea.removeFromTop(20));


    mInputGainSlider.setBounds(leftSliderArea);
    mOutputGainSlider.setBounds(rightSliderArea);

    compressorMenu.setBounds(ComboBoxArea);
    topArea.removeFromLeft(topSliderAreaWidth);
    topArea.removeFromRight(topSliderAreaWidth);
    mSpectrumView.setBounds(topArea);
    mThresholdEditor.setBounds(topArea);



   // mSpectrumView.addSpectrumBuffer(audioProcessor.mFFTCalculator->getBuffer());
   // 



    auto sliderAreaWidth = area.getWidth() / 4;
    auto spacing = 5;
    area.removeFromTop(20);
    area.removeFromBottom(20);
    auto textArea = area.removeFromTop(20);
    texts[2].setBounds(textArea.removeFromLeft(sliderAreaWidth));
    texts[0].setBounds(textArea.removeFromLeft(sliderAreaWidth));
    texts[1].setBounds(textArea.removeFromLeft(sliderAreaWidth));
    texts[5].setBounds(textArea.removeFromLeft(sliderAreaWidth));


    mRatioSlider.setBounds(area.removeFromLeft(sliderAreaWidth).reduced(spacing));
    mAttackSlider.setBounds(area.removeFromLeft(sliderAreaWidth).reduced(spacing));
    mReleaseSlider.setBounds(area.removeFromLeft(sliderAreaWidth).reduced(spacing));
    mMixSlider.setBounds(area.removeFromLeft(sliderAreaWidth).reduced(spacing));
}
