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
File: RatiosBar.cpp
Implements RatiosBar GUI object.
*/
#include "GuiItems.hpp"


RatiosBarItem::RatiosBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
{
    addAndMakeVisible (label);
    startTimerHz(30);
}

void RatiosBarItem::refresh()
{
    int id = getProperty("osc_id");
    if(id>=1 && id<=6)
    {
        auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
        if(guiconfig)
        {
            auto coarse = guiconfig->fm_coarse[id-1];
            auto fine = guiconfig->fm_fine[id-1];

            float f = (coarse == 0) ? 0.5f : (float)coarse;
            f = f + (f / 100) * ((float)fine);
            label.setText(juce::String("f = ") + juce::String(f),juce::dontSendNotification);
        }
    }
}

std::vector<foleys::SettableProperty> RatiosBarItem::getSettableProperties() const
{
    std::vector<foleys::SettableProperty> newProperties;

    newProperties.push_back ({ configNode, "osc_id", foleys::SettableProperty::Number, 1, {} });

    return newProperties;
}

void RatiosBarItem::timerCallback()
{
    refresh();
}

// Sets label callback and updates model list.
void RatiosBarItem::update()
{
    refresh();
}