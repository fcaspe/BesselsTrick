/*
 ==============================================================================
    Copyright (c) 2023 Franco Caspe
    All rights reserved.

    **BSD 3-Clause License**

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

 ==============================================================================

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
*/

/*
File: DrawableLabel.cpp
Implements DrawableLabel GUI object.
*/

#include "GuiItems.hpp"
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

