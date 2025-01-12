#include "input_action.h"
#include "nit_pch.h"
#include "core/asset.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif
#include "input_registry.h"

namespace nit
{
    void serialize(const InputAction* input_action, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "key" <<                YAML::Value << enum_to_string<GamepadKeys>(input_action->m_Key);
        emitter << YAML::Key << "paired_input_key_x" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyX);
        emitter << YAML::Key << "paired_input_key_y" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyY);
        emitter << YAML::Key << "paired_input_key_z" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyZ);
        emitter << YAML::Key << "paired_input_key_w" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyW);
        emitter << YAML::Key << "input_type" <<         YAML::Value << enum_to_string<InputType>(input_action->m_InputType);
        emitter << YAML::Key << "trigger_type" <<       YAML::Value << enum_to_string<TriggerType>(input_action->m_TriggerType);

        emitter << YAML::Key << "input_modifiers" << YAML::Value << YAML::BeginMap;

        InputRegistry* input_registry = input_registry_get_instance();
        for (u8 i = 0; i < input_registry->next_input_modifier_type_index - 1; ++i)
        {
            auto& input_modifier_pool = input_registry->input_modifier_pool[i];
            auto& data_pool = input_modifier_pool.data_pool;

            if (!data_pool.type->fn_invoke_deserialize
                || !data_pool.type->fn_invoke_serialize
                || !delegate_invoke(input_modifier_pool.fn_is_in_input_action, input_action))
            {
                continue;
            }

            emitter << YAML::Key << data_pool.type->name << YAML::Value << YAML::BeginMap;

            void* raw_data = pool_get_raw_data(&data_pool, input_action->id);
            serialize(data_pool.type, raw_data, emitter);

            emitter << YAML::EndMap;
        }

        emitter << YAML::EndMap;
    }

    void deserialize(InputAction* input_action, const YAML::Node& node)
    {
        InputRegistry* input_registry = input_registry_get_instance();

        input_action->id = input_registry->available_input_actions.front(); 
        input_registry->available_input_actions.pop();
        input_action->m_Key =             enum_from_string<GamepadKeys>(node["key"].as<String>());
        input_action->m_PairedInputKeyX = enum_from_string<GamepadKeys>(node["paired_input_key_x"].as<String>());
        input_action->m_PairedInputKeyY = enum_from_string<GamepadKeys>(node["paired_input_key_y"].as<String>());
        input_action->m_PairedInputKeyZ = enum_from_string<GamepadKeys>(node["paired_input_key_z"].as<String>());
        input_action->m_PairedInputKeyW = enum_from_string<GamepadKeys>(node["paired_input_key_w"].as<String>());
        input_action->m_InputType =       enum_from_string<InputType>(node["input_type"].as<String>());
        input_action->m_TriggerType =     enum_from_string<TriggerType>(node["trigger_type"].as<String>());

        const YAML::Node& modifiers_node = node["input_modifiers"];

        const InputAction* const_input_action = input_action;
        for (const auto& action_node_child : modifiers_node)
        {
            const YAML::Node& modifier_node = action_node_child.second;
            String type_name = action_node_child.first.as<String>();
            auto* input_modifier_pool = input_find_modifier_pool(type_get(type_name));
            auto& data_pool = input_modifier_pool->data_pool;
            void* null_data = nullptr;
            delegate_invoke(input_modifier_pool->fn_add_to_input_action, input_action, null_data, false);
            void* modifier_data = delegate_invoke(input_modifier_pool->fn_get_from_input_action, const_input_action);
            deserialize(data_pool.type, modifier_data, modifier_node);
            InputModifierAddedArgs args;
            args.input_action = input_action;
            args.type = input_modifier_pool->data_pool.type;
            event_broadcast<const InputModifierAddedArgs&>(input_registry->input_modifier_added_event, args);
        }
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(InputAction* input_action)
    {

        editor_draw_enum_combo("Key", input_action->m_Key);
        editor_draw_enum_combo("Paired Key X", input_action->m_PairedInputKeyX);
        editor_draw_enum_combo("Paired Key Y", input_action->m_PairedInputKeyY);
        editor_draw_enum_combo("Paired Key Z", input_action->m_PairedInputKeyZ);
        editor_draw_enum_combo("Paired Key W", input_action->m_PairedInputKeyW);
        editor_draw_enum_combo("Input Type", input_action->m_InputType);
        editor_draw_enum_combo("Trigger Type", input_action->m_TriggerType);

        InputRegistry* input_registry = input_registry_get_instance();

        // if delegate needs a const InputAction* using a non const doesnt compile
        const InputAction* const_input_action = (const InputAction*)input_action;
        for (u32 i = 0; i < input_registry->next_input_modifier_type_index - 1; ++i)
        {
            InputModifierPool* modifier_pool = &input_registry->input_modifier_pool[i];
            if (!delegate_invoke(modifier_pool->fn_is_in_input_action, const_input_action))
            {
                continue;
            }

            Type* modifier_type = modifier_pool->data_pool.type;

            bool is_expanded = ImGui::TreeNodeEx(modifier_type->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            ImGui::SameLine();

            ImGui::PushID(i);
            if (ImGui::Button("+"))
            {
                ImGui::OpenPopup("Input Modifier Settings");
            }

            bool remove_input_modifier = false;

            if (ImGui::BeginPopup("Input Modifier Settings"))
            {
                if (ImGui::MenuItem("Remove Input Modifier"))
                {
                    remove_input_modifier = true;
                }

                ImGui::EndPopup();
            }
            ImGui::PopID();

            if (is_expanded)
            {
                ImGui::Spacing();

                if (modifier_pool->data_pool.type->fn_invoke_draw_editor)
                {
                    void* data = pool_get_raw_data(&modifier_pool->data_pool, input_action->id);
                    NIT_CHECK(data);
                    type_draw_editor(modifier_type, data);
                }

                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TreePop();
            }

            if (remove_input_modifier)
            {
                delegate_invoke(modifier_pool->fn_remove_from_input_action, input_action);
            }
        }
        if (editor_draw_centered_button("Add Input Modifier"))
        {
            ImGui::OpenPopup("Add Input Modifier");
        }

        if (ImGui::BeginPopup("Add Input Modifier"))
        {
            for (u32 i = 0; i < input_registry_get_instance()->next_input_modifier_type_index - 1; ++i)
            {
                InputModifierPool* pool = &input_registry_get_instance()->input_modifier_pool[i];
                if (delegate_invoke(pool->fn_is_in_input_action, const_input_action))
                {
                    continue;
                }

                if (ImGui::MenuItem(pool->data_pool.type->name.c_str()))
                {
                    void* null_data = nullptr;
                    delegate_invoke(pool->fn_add_to_input_action, input_action, null_data, true);
                }
            }
            ImGui::EndPopup();
        }
    }
#endif

    void print_key(InputAction* input_action)
    {
        NIT_LOG_TRACE("%s", enum_to_string<GamepadKeys>(input_action->m_Key).c_str());
    }

    void register_input_action_asset()
    {
        enum_register<GamepadKeys>();
        enum_register_value<GamepadKeys>("GamepadButtonNONE", GamepadKeys::GamepadButtonNONE);
        enum_register_value<GamepadKeys>("GamepadButtonSouth", GamepadKeys::GamepadButtonSouth);
        enum_register_value<GamepadKeys>("GamepadButtonEast", GamepadKeys::GamepadButtonEast);
        enum_register_value<GamepadKeys>("GamepadButtonWest", GamepadKeys::GamepadButtonWest);
        enum_register_value<GamepadKeys>("GamepadButtonNorth", GamepadKeys::GamepadButtonNorth);
        enum_register_value<GamepadKeys>("GamepadButtonLeftBumper", GamepadKeys::GamepadButtonLeftBumper);
        enum_register_value<GamepadKeys>("GamepadButtonRightBumper", GamepadKeys::GamepadButtonRightBumper);
        enum_register_value<GamepadKeys>("GamepadButtonSelect", GamepadKeys::GamepadButtonSelect);
        enum_register_value<GamepadKeys>("GamepadButtonStart", GamepadKeys::GamepadButtonStart);
        enum_register_value<GamepadKeys>("GamepadButtonHome", GamepadKeys::GamepadButtonHome);
        enum_register_value<GamepadKeys>("GamepadButtonLeftThumb", GamepadKeys::GamepadButtonLeftThumb);
        enum_register_value<GamepadKeys>("GamepadButtonRightThumb", GamepadKeys::GamepadButtonRightThumb);
        enum_register_value<GamepadKeys>("GamepadButtonDpadUp", GamepadKeys::GamepadButtonDpadUp);
        enum_register_value<GamepadKeys>("GamepadButtonDpadRight", GamepadKeys::GamepadButtonDpadRight);
        enum_register_value<GamepadKeys>("GamepadButtonDpadDown", GamepadKeys::GamepadButtonDpadDown);
        enum_register_value<GamepadKeys>("GamepadButtonDpadLeft", GamepadKeys::GamepadButtonDpadLeft);
        enum_register_value<GamepadKeys>("GamepadAxisLeftTrigger", GamepadKeys::GamepadAxisLeftTrigger);
        enum_register_value<GamepadKeys>("GamepadAxisRightTrigger", GamepadKeys::GamepadAxisRightTrigger);
        enum_register_value<GamepadKeys>("GamepadAxisLeftX", GamepadKeys::GamepadAxisLeftX);
        enum_register_value<GamepadKeys>("GamepadAxisLeftY", GamepadKeys::GamepadAxisLeftY);
        enum_register_value<GamepadKeys>("GamepadAxisLeft2D", GamepadKeys::GamepadAxisLeft2D);
        enum_register_value<GamepadKeys>("GamepadAxisRightX", GamepadKeys::GamepadAxisRightX);
        enum_register_value<GamepadKeys>("GamepadAxisRightY", GamepadKeys::GamepadAxisRightY);
        enum_register_value<GamepadKeys>("GamepadAxisRight2D", GamepadKeys::GamepadAxisRight2D);
        enum_register_value<GamepadKeys>("GamepadAxisGyroX", GamepadKeys::GamepadAxisGyroX);
        enum_register_value<GamepadKeys>("GamepadAxisGyroY", GamepadKeys::GamepadAxisGyroY);
        enum_register_value<GamepadKeys>("GamepadAxisGyroZ", GamepadKeys::GamepadAxisGyroZ);
        enum_register_value<GamepadKeys>("GamepadAxisGyro3D", GamepadKeys::GamepadAxisGyro3D);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelX", GamepadKeys::GamepadAxisTotalAccelX);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelY", GamepadKeys::GamepadAxisTotalAccelY);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelZ", GamepadKeys::GamepadAxisTotalAccelZ);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccel3D", GamepadKeys::GamepadAxisTotalAccel3D);
        enum_register_value<GamepadKeys>("GamepadAxisAccelX", GamepadKeys::GamepadAxisAccelX);
        enum_register_value<GamepadKeys>("GamepadAxisAccelY", GamepadKeys::GamepadAxisAccelY);
        enum_register_value<GamepadKeys>("GamepadAxisAccelZ", GamepadKeys::GamepadAxisAccelZ);
        enum_register_value<GamepadKeys>("GamepadAxisAccel3D", GamepadKeys::GamepadAxisAccel3D);
        enum_register_value<GamepadKeys>("GamepadAxisGravX", GamepadKeys::GamepadAxisGravX);
        enum_register_value<GamepadKeys>("GamepadAxisGravY", GamepadKeys::GamepadAxisGravY);
        enum_register_value<GamepadKeys>("GamepadAxisGravZ", GamepadKeys::GamepadAxisGravZ);
        enum_register_value<GamepadKeys>("GamepadAxisGrav3D", GamepadKeys::GamepadAxisGrav3D);
        enum_register_value<GamepadKeys>("GamepadAxisQuatX", GamepadKeys::GamepadAxisQuatX);
        enum_register_value<GamepadKeys>("GamepadAxisQuatY", GamepadKeys::GamepadAxisQuatY);
        enum_register_value<GamepadKeys>("GamepadAxisQuatZ", GamepadKeys::GamepadAxisQuatZ);
        enum_register_value<GamepadKeys>("GamepadAxisQuatW", GamepadKeys::GamepadAxisQuatW);
        enum_register_value<GamepadKeys>("GamepadAxisQuat4D", GamepadKeys::GamepadAxisQuat4D);
        enum_register_value<GamepadKeys>("SwitchButtonCapture", GamepadKeys::SwitchButtonCapture);
        enum_register_value<GamepadKeys>("SwitchButtonLeftSL", GamepadKeys::SwitchButtonLeftSL);
        enum_register_value<GamepadKeys>("SwitchButtonLeftSR", GamepadKeys::SwitchButtonLeftSR);
        enum_register_value<GamepadKeys>("SwitchButtonRightSL", GamepadKeys::SwitchButtonRightSL);
        enum_register_value<GamepadKeys>("SwitchButtonRightSR", GamepadKeys::SwitchButtonRightSR);
        enum_register_value<GamepadKeys>("SwitchButtonSL", GamepadKeys::SwitchButtonSL);
        enum_register_value<GamepadKeys>("SwitchButtonSR", GamepadKeys::SwitchButtonSR);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroX", GamepadKeys::SwitchAxisLeftGyroX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroY", GamepadKeys::SwitchAxisLeftGyroY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroZ", GamepadKeys::SwitchAxisLeftGyroZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyro3D", GamepadKeys::SwitchAxisLeftGyro3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelX", GamepadKeys::SwitchAxisLeftAccelX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelY", GamepadKeys::SwitchAxisLeftAccelY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelZ", GamepadKeys::SwitchAxisLeftAccelZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccel3D", GamepadKeys::SwitchAxisLeftAccel3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravX", GamepadKeys::SwitchAxisLeftGravX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravY", GamepadKeys::SwitchAxisLeftGravY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravZ", GamepadKeys::SwitchAxisLeftGravZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGrav3D", GamepadKeys::SwitchAxisLeftGrav3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatX", GamepadKeys::SwitchAxisLeftQuatX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatY", GamepadKeys::SwitchAxisLeftQuatY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatZ", GamepadKeys::SwitchAxisLeftQuatZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatW", GamepadKeys::SwitchAxisLeftQuatW);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuat4D", GamepadKeys::SwitchAxisLeftQuat4D);
        enum_register_value<GamepadKeys>("DualShockButtonCreate", GamepadKeys::DualShockButtonCreate);
        enum_register_value<GamepadKeys>("DualShockButtonMic", GamepadKeys::DualShockButtonMic);
        enum_register_value<GamepadKeys>("DualShockButtonTouchpadDown", GamepadKeys::DualShockButtonTouchpadDown);
        enum_register_value<GamepadKeys>("DualShockButtonFirstTouchpad", GamepadKeys::DualShockButtonFirstTouchpad);
        enum_register_value<GamepadKeys>("DualShockButtonSecondTouchpad", GamepadKeys::DualShockButtonSecondTouchpad);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpadX", GamepadKeys::DualShockAxisFirstTouchpadX);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpadY", GamepadKeys::DualShockAxisFirstTouchpadY);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpad2D", GamepadKeys::DualShockAxisFirstTouchpad2D);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpadX", GamepadKeys::DualShockAxisSecondTouchpadX);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpadY", GamepadKeys::DualShockAxisSecondTouchpadY);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpad2D", GamepadKeys::DualShockAxisSecondTouchpad2D);
        enum_register_value<GamepadKeys>("DualShockAxisDpadUp", GamepadKeys::DualShockAxisDpadUp);
        enum_register_value<GamepadKeys>("DualShockAxisDpadRight", GamepadKeys::DualShockAxisDpadRight);
        enum_register_value<GamepadKeys>("DualShockAxisDpadDown", GamepadKeys::DualShockAxisDpadDown);
        enum_register_value<GamepadKeys>("DualShockAxisDpadLeft", GamepadKeys::DualShockAxisDpadLeft);
        enum_register_value<GamepadKeys>("DualShockAxisSouth", GamepadKeys::DualShockAxisSouth);
        enum_register_value<GamepadKeys>("DualShockAxisEast", GamepadKeys::DualShockAxisEast);
        enum_register_value<GamepadKeys>("DualShockAxisWest", GamepadKeys::DualShockAxisWest);
        enum_register_value<GamepadKeys>("DualShockAxisNorth", GamepadKeys::DualShockAxisNorth);
        enum_register_value<GamepadKeys>("DualShockAxisLeftBumper", GamepadKeys::DualShockAxisLeftBumper);
        enum_register_value<GamepadKeys>("DualShockAxisRightBumper", GamepadKeys::DualShockAxisRightBumper);

        enum_register<InputType>();
        enum_register_value<InputType>("Digital", InputType::Digital);
        enum_register_value<InputType>("Axis1D", InputType::Axis1D);
        enum_register_value<InputType>("Axis2D", InputType::Axis2D);
        enum_register_value<InputType>("Axis3D", InputType::Axis3D);
        enum_register_value<InputType>("Axis4D", InputType::Axis4D);

        enum_register<TriggerType>();
        enum_register_value<TriggerType>("Down", TriggerType::Down);
        enum_register_value<TriggerType>("Pressed", TriggerType::Pressed);
        enum_register_value<TriggerType>("Released", TriggerType::Released);

        u32 max_input_actions = input_registry_get_instance()->max_input_actions;
        asset_register_type<InputAction>({
            .fn_serialize = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor),
            .max_elements = max_input_actions,
            });
    }

    
}

