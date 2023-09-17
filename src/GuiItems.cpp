#include "GuiItems.hpp"
#include "BinaryData.h"

DrawableLabel::DrawableLabel()
{
    // make sure you define some default colour, otherwise the juce lookup will choke
    setColour (backgroundColourId, juce::Colours::black);
    setColour (drawColourId, juce::Colours::green);
    setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));

    startTimerHz (30);
}

void DrawableLabel::setAlgorithm(int number)
{
    if(number > 0 && number < 33)
        _algorithm = number;
}

void DrawableLabel::paint (juce::Graphics& g)
{
    juce::Image myImage;
    
    myImage = juce::ImageFileFormat::loadFrom(
                            _algoplots[_algorithm-1],
                            _algoplotSizes[_algorithm-1]);

    g.drawImage(myImage,0,0,getWidth(),getHeight(),
                0,0,myImage.getWidth(),myImage.getHeight());
}

void DrawableLabel::timerCallback()
{
    repaint();
}
