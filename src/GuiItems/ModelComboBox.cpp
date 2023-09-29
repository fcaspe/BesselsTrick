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
File: ModelComboBox.cpp
Implements ModelComboBox GUI object.
*/
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
