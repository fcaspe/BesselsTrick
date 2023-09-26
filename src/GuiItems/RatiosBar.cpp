#include "../GuiItems.hpp"


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