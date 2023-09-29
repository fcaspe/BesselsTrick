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
File: GuiItems.hpp

Declaration of Special GUI items, including:
    DrawableLabel - for displaying FM algorithm.
    ModelComboBox - for listing patches.
    StatusBar - for printing indications of use in GUI.
    RatiosBar - for printing oscillator frequency ratios in GUI.
*/

// Avoid using namespace juce, as it collides with libtorch.
#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "PluginProcessor.hpp"

// gui component for displaying the FM algorithm
class DrawableLabel : public juce::Component
{
public:
    enum ColourIDs
    {
        // we are safe from collissions, because we set the colours on every component directly from the stylesheet
        backgroundColourId,
        drawColourId,
        fillColourId
    };

    DrawableLabel();
    void setAlgorithm(int number);
    void paint (juce::Graphics& g) override;

private:
    int _algorithm = 1;
    const char *_algoplots[32] = {
        BinaryData::algo_1_png,BinaryData::algo_2_png,BinaryData::algo_3_png,BinaryData::algo_4_png,
        BinaryData::algo_5_png,BinaryData::algo_6_png,BinaryData::algo_7_png,BinaryData::algo_8_png,
        BinaryData::algo_9_png,BinaryData::algo_10_png,BinaryData::algo_11_png,BinaryData::algo_12_png,
        BinaryData::algo_13_png,BinaryData::algo_14_png,BinaryData::algo_15_png,BinaryData::algo_16_png,
        BinaryData::algo_17_png,BinaryData::algo_18_png,BinaryData::algo_19_png,BinaryData::algo_20_png,
        BinaryData::algo_21_png,BinaryData::algo_22_png,BinaryData::algo_23_png,BinaryData::algo_24_png,
        BinaryData::algo_25_png,BinaryData::algo_26_png,BinaryData::algo_27_png,BinaryData::algo_28_png,
        BinaryData::algo_29_png,BinaryData::algo_30_png,BinaryData::algo_31_png,BinaryData::algo_32_png
    };

    const int _algoplotSizes[32] = {
        BinaryData::algo_1_pngSize,BinaryData::algo_2_pngSize,BinaryData::algo_3_pngSize,BinaryData::algo_4_pngSize,
        BinaryData::algo_5_pngSize,BinaryData::algo_6_pngSize,BinaryData::algo_7_pngSize,BinaryData::algo_8_pngSize,
        BinaryData::algo_9_pngSize,BinaryData::algo_10_pngSize,BinaryData::algo_11_pngSize,BinaryData::algo_12_pngSize,
        BinaryData::algo_13_pngSize,BinaryData::algo_14_pngSize,BinaryData::algo_15_pngSize,BinaryData::algo_16_pngSize,
        BinaryData::algo_17_pngSize,BinaryData::algo_18_pngSize,BinaryData::algo_19_pngSize,BinaryData::algo_20_pngSize,
        BinaryData::algo_21_pngSize,BinaryData::algo_22_pngSize,BinaryData::algo_23_pngSize,BinaryData::algo_24_pngSize,
        BinaryData::algo_25_pngSize,BinaryData::algo_26_pngSize,BinaryData::algo_27_pngSize,BinaryData::algo_28_pngSize,
        BinaryData::algo_29_pngSize,BinaryData::algo_30_pngSize,BinaryData::algo_31_pngSize,BinaryData::algo_32_pngSize
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableLabel)
};

// DrawableLabel: Shows current FM synth algorithm
class DrawableLabelItem : public foleys::GuiItem, juce::Timer
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (DrawableLabelItem)

    DrawableLabelItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);
    void update() override;
    void timerCallback() override;
    juce::Component* getWrappedComponent() override
    {
        return &drawablelabel;
    }

private:
    DrawableLabel drawablelabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableLabelItem)
};


// ModelComboBox: Displays a list of models to load.
class ModelComboBoxItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ModelComboBoxItem)

    ModelComboBoxItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);
    // Sets combobox callback and updates model list.
    void update() override;
    juce::Component* getWrappedComponent() override
    {
        return &combobox;
    }

private:
    juce::ComboBox combobox;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModelComboBoxItem)
};


// Status Bar, provides indications to users
class StatusBarItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (StatusBarItem)
    
    StatusBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);
    void update_status_message();
    // Sets label callback and updates model list.
    void update() override;
    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusBarItem)
};


// This class is creating and configuring your custom component
class RatiosBarItem : public foleys::GuiItem,
                      private juce::Timer
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (RatiosBarItem)

    RatiosBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);
    void refresh();
    std::vector<foleys::SettableProperty> getSettableProperties() const override;
    void timerCallback() override;
    // Sets label callback and updates model list.
    void update() override;
    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RatiosBarItem)
};