/*
  ==============================================================================

    SpectrumView.cpp
    Created: 24 Oct 2021 4:00:47pm
    Author:  kosty

  ==============================================================================
*/

#include "SpectrumView.h"

SpectrumView::SpectrumView(DynamifyIIAudioProcessor& p) : audioProcessor(p)
{

    setSize(200, 200);
    startTimerHz(FPS);
    
    mfreqPositions = nullptr;
    mBuffer[0] = nullptr;
    mBuffer[1] = nullptr;

    mMaxAmp[0] = -91.f;
    mMaxAmp[1] = -91.f;

    setResolution(BUFFERSIZE);
}

SpectrumView::~SpectrumView()
{
    resetBuffers();
}

void SpectrumView::timerCallback()
{
    mBuffer[0] = audioProcessor.mFTTCalculator[0]->getBuffer();
    mBuffer[1] = audioProcessor.mFTTCalculator[1]->getBuffer();
    calcMaxAmp();
    //DBG(mBuffer[0]);
    repaint();
}

void SpectrumView::paint(juce::Graphics& g)
{

    g.setColour(juce::Colour(0xff2d2d2d));

    g.drawImage(mBackground, getLocalBounds().toFloat());

    float ampRange = -90; // in dB

    if (mMaxAmp[0] > ampRange) // don't paint if it's too quiet
    {
        juce::Path spectrumPathIn;
        for (auto i = 0; i < mVisibleSize; ++i)
        {

            juce::Point<float> p((float)mfreqPositions[i + mCntrUnder20Hz - 1] * (float)getWidth(),
                mBuffer[0][i + mCntrUnder20Hz - 1] / ampRange * (float)getHeight());

            /*juce::Point<float> p((float)i/ ((float)mSize)*getWidth(),
                (mBuffer[i]+ (float)1)/ (float)2 * (float)getHeight());*/

            if (i == 0)
                spectrumPathIn.startNewSubPath(p);  // if this is the first point, starting a new path
            else
                spectrumPathIn.lineTo(p);

        }
        g.setColour(juce::Colour(0xffb3b3b3));
        juce::Path roundedPath1 = spectrumPathIn.createPathWithRoundedCorners(1.0f); //16
        g.strokePath(roundedPath1, juce::PathStrokeType(1.2f, juce::PathStrokeType::JointStyle::curved));
    }

    if (mMaxAmp[1] > ampRange)
    {
        juce::Path spectrumPathOut;
        for (auto i = 0; i < mVisibleSize; ++i)
        {
            juce::Point<float> p((float)mfreqPositions[i + mCntrUnder20Hz - 1] * (float)getWidth(),
                mBuffer[1][i + mCntrUnder20Hz - 1] / ampRange * (float)getHeight());

            if (i == 0)
                spectrumPathOut.startNewSubPath(p);
            else
                spectrumPathOut.lineTo(p);
        }
        g.setColour(juce::Colour(0xff33bfdb));
        juce::Path roundedPath2 = spectrumPathOut.createPathWithRoundedCorners(1.0f); //16
        g.strokePath(roundedPath2, juce::PathStrokeType(2.f, juce::PathStrokeType::JointStyle::curved));
    }
}

void SpectrumView::resized() // painting the grid
{
    mBackground = juce::Image(juce::Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    juce::Graphics g(mBackground);
    g.fillAll(juce::Colour(0xffb3b3b3));

    g.setColour(juce::Colour(0xff2d2d2d));
    auto area = getLocalBounds();
    const juce::Rectangle<float> areaFix = area.toFloat();
    g.fillRoundedRectangle(areaFix, 10.0f);

    // AMP GRID
    juce::Array<juce::String> ampsLabel
    {
        "",
        "-80",
        "-70",
        "-60",
        "-50",
        "-40",
        "-30",
        "-20",
        "-10",
    };
    juce::Array<float> amps
    {
        -90, -80, -70, -60, -50, -40, -30, -20, -10
    };

    int i = 0;
    for (auto am : amps)
    {
        auto normY = juce::jmap(am, -90.f, 0.f, float(getHeight()), 0.f);
        g.setColour(juce::Colour(0xff383838));
        g.drawHorizontalLine(normY, 0.f, getWidth() - 30);
        g.setColour(juce::Colour(0xff545454));
        g.drawText(ampsLabel[i], getWidth() - 30, normY - 10, 30, 20, juce::Justification::centred);
        i++;
    }


    // FREQ GRID
    juce::Array<float> freqs
    {
            30, 40, 50, 60, 70, 80, 90, 100,
        200, 300, 400, 500, 600, 700, 800, 900, 1000,
        2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
        20000
    };
    juce::Array<juce::String> freqsLabel
    {
        "20",
        "100",
        "1k",
        "10k",
        "20k"
    };

    g.setColour(juce::Colour(0xff545454));
    g.drawText(freqsLabel[0], 5.f, getHeight() - 20, 40, 20, juce::Justification::centredLeft);
    for (auto f : freqs)
    {
        g.setColour(juce::Colour(0xff545454));

        auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
        if (f == 100)
        {
            g.drawText(freqsLabel[1], getWidth() * normX - 20, getHeight() - 20, 40, 20, juce::Justification::centred);
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight() - 20);
        }
        else if (f == 1000)
        {
            g.drawText(freqsLabel[2], getWidth() * normX - 20, getHeight() - 20, 40, 20, juce::Justification::centred);
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight() - 20);
        }
        else if (f == 10000)
        {
            g.drawText(freqsLabel[3], getWidth() * normX - 20, getHeight() - 20, 40, 20, juce::Justification::centred);
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight() - 20);
        }
        else if (f == 20000)
        {
            g.drawText(freqsLabel[4], getWidth() * normX - 25, getHeight() - 20, 40, 20, juce::Justification::centredLeft);
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight() - 20);
        }
        else if (f == 90 || f == 9000)
        {
            g.setColour(juce::Colour(0xff383838));
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight() - 20);
        }
        else
        {
            g.setColour(juce::Colour(0xff383838));
            g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
        }
    }
}

void SpectrumView::setResolution(int size)
{
    mSize = size / 2;
    //mSize = size;                                
    
    mfreqPositions = new float[mSize];
    float* tmpFreqs = audioProcessor.mFTTCalculator[0]->getFreqs();
    
    mCntrUnder20Hz = 0;
    mCntrAbove20kHz = 0;
    int c = 0;
    for (auto i = 0; i < mSize; ++i)
    {
        if (tmpFreqs[i] > 20.f && tmpFreqs[i] <= 20000.f)
        {
            mfreqPositions[i] = log10(tmpFreqs[i]);
            mfreqPositions[i] = (mfreqPositions[i] - log10(20)) / (log10(20000) - log10(20));
            c++;
        }
        if (tmpFreqs[i] <= 20.f)
        {
            mCntrUnder20Hz++;
            mfreqPositions[i] = 0.f;
        }
        if (tmpFreqs[i] > 20000.f)
        {
            mCntrAbove20kHz++;
            if (mCntrAbove20kHz == 1)
                mfreqPositions[i] = 1.f;
        }
       
    }
    mVisibleSize = c;
}

void SpectrumView::calcMaxAmp()
{
    mMaxAmp[0] = -91;
    mMaxAmp[1] = -91;
    for (auto i = 0; i < mSize; ++i)
    {
        if (mBuffer[0][i] >= mMaxAmp[0])
            mMaxAmp[0] = mBuffer[0][i];
        if (mBuffer[1][i] >= mMaxAmp[1])
            mMaxAmp[1] = mBuffer[1][i];
    }
        
}

void SpectrumView::resetBuffers()
{
    if (mfreqPositions != nullptr)
    {
        delete[] mfreqPositions;
        mfreqPositions = nullptr;
    }
    //mBuffer[0] = nullptr;
    //mBuffer[1] = nullptr;
}
