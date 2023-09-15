// Avoid using namespace juce, as it collides with libtorch.
#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"
#include <foleys_gui_magic/foleys_gui_magic.h>

// Some nice example drawing
class DrawableLabel : public juce::Component,
                      private juce::Timer
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
    void timerCallback() override;
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

// This class is creating and configuring your custom component
class DrawableLabelItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (DrawableLabelItem)

    DrawableLabelItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"drawablelabel-background", DrawableLabel::backgroundColourId},
            {"drawablelabel-draw", DrawableLabel::drawColourId},
            {"drawablelabel-fill", DrawableLabel::fillColourId} });

        addAndMakeVisible (drawablelabel);
    }

    // Override update() to set the GUI values to your custom component
    void update() override
    {
        
        juce::var *alg = magicBuilder.getMagicState().getObjectWithType<juce::var>("algorithm");
        if(alg != nullptr)
            {
            //std::cout << "[UPDATE] alg: " << int(*alg)<< std::endl;
            drawablelabel.setAlgorithm (int(*alg));
            }
    }

    juce::Component* getWrappedComponent() override
    {
        return &drawablelabel;
    }

private:
    DrawableLabel drawablelabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableLabelItem)
};


// This class is creating and configuring your custom component
class ModelComboBoxItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ModelComboBoxItem)

    ModelComboBoxItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {

        addAndMakeVisible (combobox);
    }

    // Override update() to set the GUI values to your custom component
    void update() override
    {
        std::cout << "[UPDATE CBOX] Update()" << std::endl;
        combobox.addItem("Set by update()",1);

    }

    juce::Component* getWrappedComponent() override
    {
        return &combobox;
    }

private:
    juce::ComboBox combobox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModelComboBoxItem)
};