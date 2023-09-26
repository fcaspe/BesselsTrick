#include "../GuiItems.hpp"
#include "BinaryData.h"

DrawableLabel::DrawableLabel()
{
    // make sure you define some default colour, otherwise the juce lookup will choke
    setColour (backgroundColourId, juce::Colours::black);
    setColour (drawColourId, juce::Colours::green);
    setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));
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


DrawableLabelItem::DrawableLabelItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node): foleys::GuiItem (builder, node)
{
    // Create the colour names to have them configurable
    setColourTranslation ({
        {"drawablelabel-background", DrawableLabel::backgroundColourId},
        {"drawablelabel-draw", DrawableLabel::drawColourId},
        {"drawablelabel-fill", DrawableLabel::fillColourId} });

    addAndMakeVisible (drawablelabel);
    startTimerHz (30);
}

// Override update() to set the GUI values to your custom component
void DrawableLabelItem::update()
{
    
    auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
    if(guiconfig)
        {
        //std::cout << "[UPDATE] alg: " << int(*alg)<< std::endl;
        drawablelabel.setAlgorithm (guiconfig->fm_algo);
        }
    drawablelabel.repaint();
}

void DrawableLabelItem::timerCallback()
{
    update();
}

