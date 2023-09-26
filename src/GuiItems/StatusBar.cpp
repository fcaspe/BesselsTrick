#include "../GuiItems.hpp"



StatusBarItem::StatusBarItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
{
    addAndMakeVisible (label);
}

void StatusBarItem::update_status_message()
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
void StatusBarItem::update()
{
    update_status_message();
    auto *guiconfig = magicBuilder.getMagicState().getObjectWithType<PluginGUIConfig>("guiconfig");
    if(!guiconfig) return;
    label.setText(guiconfig->status,juce::dontSendNotification);
}