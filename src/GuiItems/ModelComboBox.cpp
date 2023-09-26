#include "../GuiItems.hpp"

ModelComboBoxItem::ModelComboBoxItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node): foleys::GuiItem (builder, node)
{
    addAndMakeVisible (combobox);
}

// Sets combobox callback and updates model list.
void ModelComboBoxItem::update()
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
