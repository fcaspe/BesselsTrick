// Avoid using namespace juce, as it collides with libtorch.
#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "PluginProcessor.h"
// Some nice example drawing
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

// This class is creating and configuring your custom component
class DrawableLabelItem : public foleys::GuiItem, juce::Timer
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
        startTimerHz (30);
    }

    // Override update() to set the GUI values to your custom component
    void update() override
    {
        
        auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
        if(guiconfig)
            {
            //std::cout << "[UPDATE] alg: " << int(*alg)<< std::endl;
            drawablelabel.setAlgorithm (guiconfig->fm_algo);
            }
        drawablelabel.repaint();
    }

    void timerCallback() override
    {
        update();
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

    // Sets combobox callback and updates model list.
    void update() override
    {
        //std::cout << "[MODEL CBOX] Update()" << std::endl;
        if (auto* processor = dynamic_cast<FMTTProcessor*>(magicBuilder.getMagicState().getProcessor()))
            {
            combobox.onChange = [&] {
                if (auto* proc = dynamic_cast<FMTTProcessor*>(magicBuilder.getMagicState().getProcessor()))
                {
                    const auto selected_entry = combobox.getSelectedId() - 1;
                    // Stop Audio Processing Thread ( It can crash when re-loading model )
                    proc->suspendProcessing(true);
                    proc->reload_model(selected_entry);
                    proc->updateKnobs();
                    proc->storeToValTree(ValTree_IDs::gui_params,"SelectedID",juce::var(combobox.getSelectedId()));
                    proc->suspendProcessing(false);
                
                auto *status_label = magicBuilder.findGuiItemWithId("lbl_status");
                if(status_label) status_label->update();
                }

            };
            combobox.clear(false);  // Remove all previous items
            int menu_idx = 1;
            auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
            if(!guiconfig) return;
            // Update ComboBox entries
            for (std::string menuentry : guiconfig->modelnames) 
                {
                combobox.addItem(menuentry, menu_idx);
                menu_idx++;
                }
            // Fetch Selected ID from ValTree
            auto child_tree = magicBuilder.getMagicState().getValueTree().
                getChildWithName(ValTree_IDs::gui_params);
            if(child_tree.isValid())
                {
                int selected_id = child_tree.getProperty("SelectedID");
                combobox.setSelectedId(selected_id,juce::dontSendNotification);
                }
            }
    }

    juce::Component* getWrappedComponent() override
    {
        return &combobox;
    }

private:
    juce::ComboBox combobox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModelComboBoxItem)
};


// This class is creating and configuring your custom component
class StatusBarItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (StatusBarItem)

    StatusBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        addAndMakeVisible (label);
    }

   void update_status_message()
    {
    if (auto* processor = dynamic_cast<FMTTProcessor*>(magicBuilder.getMagicState().getProcessor()))
    {
        auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
        if(guiconfig)
        {
            // Update Status message
            if(guiconfig->modelnames.size()== 0)
            {
                guiconfig->status = "Empty model list. Open pretrained directory.";
            }
            else
            {
                // Check if a model is loaded
                if(processor->_model.get() == nullptr)
                    guiconfig->status = "Select a model from list.";
                else
                    guiconfig->status = "Ready to play!";       
            }
        }
    }
    return;
    }

    // Sets label callback and updates model list.
    void update() override
    {
        update_status_message();
        auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
        if(!guiconfig) return;
        label.setText(guiconfig->status,juce::dontSendNotification);
    }
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

    RatiosBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        addAndMakeVisible (label);
        startTimerHz(30);
    }

    void refresh()
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

    std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        std::vector<foleys::SettableProperty> newProperties;

        newProperties.push_back ({ configNode, "osc_id", foleys::SettableProperty::Number, 1, {} });

        return newProperties;
    }

    void timerCallback() override
    {
        refresh();
    }

    // Sets label callback and updates model list.
    void update() override
    {
        refresh();
    }
    juce::Component* getWrappedComponent() override
    {
        return &label;
    }

private:
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RatiosBarItem)
};