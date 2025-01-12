#include "input_registry.h"
#include "core/engine.h"
#include "input_action.h"
#include "core/asset.h"
#include "JoyShockLibrary.h"

namespace nit
{
#define NIT_CHECK_INPUT_REGISTRY_CREATED NIT_CHECK(input_registry)

    InputRegistry* input_registry = nullptr;


    // ------------------------
    // INPUT 
    // ------------------------

    Map<GamepadKeys, InputContent> controller_state;
    Map<GamepadKeys, InputActionContext> input_action_context_map;

    void handle_button_press(bool pressed, GamepadKeys key, i32 player_id);
    void handle_axis(f32 value, GamepadKeys key, i32 player_id);
    void on_controller_analog(InputAction* input_action, f32 analog_value);
    void on_controller_vector2(InputAction* input_action, const Vector2& vector2_value);
    void on_controller_vector3(InputAction* input_action, const Vector3& vector3_value);
    void on_controller_vector4(InputAction* input_action, const Vector4& vector4_value);
    void on_controller_button_pressed(InputAction* input_action, bool is_repeat);
    void on_controller_button_released(InputAction* input_action, bool is_repeat);


    // ------------------------
    // JOYSHOCK 
    // ------------------------

    i32 lastAssignedInputHandle;
    Map<i32, GamepadKeys> buttonMap;
    Array<JoyShockDeviceEntry> joyShockDevices;
    UnorderedSet<i32> connectedDeviceIds;
    i32 connectedDevices = 0;

    constexpr i32 NUM_BUTTONS = 17;
    constexpr float REPEAT_RUMBLE_TIME = 2.f;

    //JoyShockDeviceConnected m_JoyShockDeviceConnected;
    //JoyShockDeviceDisconnected m_JoyShockDeviceDisconnected;

    void init_joyshock();
    void handle_joyshock_controller_events(i32 player_id, i32 device_id, i32 device_type, bool is_joycon_pair, i32& last_button_data);



    static ListenerAction start();
    static ListenerAction update();

    void input_registry_set_instance(InputRegistry* input_registry_instance)
    {
        if (input_registry)
        {
            NIT_CHECK_MSG(false, "InputRegistry already created!");
            return;
        }
        if (!input_registry_instance)
        {
            NIT_CHECK_MSG(false, "InputRegistry null!");
            return;
        }
        input_registry = input_registry_instance;
    }

    InputRegistry* input_registry_get_instance()
    {
        NIT_CHECK_INPUT_REGISTRY_CREATED

        return input_registry;
    }

    void input_registry_init()
    {
        NIT_CHECK_INPUT_REGISTRY_CREATED
        // create_input
        input_registry->input_modifier_pool = new InputModifierPool[NIT_MAX_INPUT_MODIFIER_TYPES];

        for (u32 i = 0; i < input_registry->max_input_actions; ++i)
        {
            input_registry->available_input_actions.push(i);
        }

        engine_event(Stage::Start) += EngineListener::create(start);
        engine_event(Stage::Update) += EngineListener::create(update);
    }

    void input_registry_finish()
    {
        NIT_CHECK_INPUT_REGISTRY_CREATED

        for (u32 i = 0; i < input_registry->next_input_modifier_type_index; ++i)
        {
            InputModifierPool& data = input_registry->input_modifier_pool[i];
            pool_free(&data.data_pool);
        }
    }

    InputModifierPool* input_find_modifier_pool(const Type* type)
    {
        NIT_CHECK_INPUT_REGISTRY_CREATED
        for (u32 i = 0; i < input_registry->next_input_modifier_type_index; ++i)
        {
            InputModifierPool& input_modifier_pool = input_registry->input_modifier_pool[i];
            if (input_modifier_pool.data_pool.type == type)
            {
                return &input_modifier_pool;
            }
        }
        return nullptr;
    }

    bool m_IsEnabled = true;

    bool IsKeyPressed(u32 key)
    {
        if (!m_IsEnabled) return false;
        const auto state = input_get_key(key);
        return state == GLFW_PRESS;
    }

    void GetButtonState(GamepadKeys key, InputContent& input_content)
    {
        if (controller_state.contains(key))
        {
            input_content = controller_state[key];
        }
        else
        {
            input_content = InputContent();
        }
    }

    void get_input_action_context(GamepadKeys key, InputActionContext& input_action_context)
    {
        if (input_action_context_map.contains(key))
        {
            input_action_context = input_action_context_map[key];
        }
        else
        {
            input_action_context = InputActionContext();
        }
    }

    void SetEnabled(const bool consumed)
    {
        m_IsEnabled = consumed;
    }

    bool IsEnabled()
    {
        return m_IsEnabled;
    }

    void init_joyshock()
    {
        buttonMap[0x00001] = GamepadKeys::GamepadButtonDpadUp;
        buttonMap[0x00002] = GamepadKeys::GamepadButtonDpadDown;
        buttonMap[0x00004] = GamepadKeys::GamepadButtonDpadLeft;
        buttonMap[0x00008] = GamepadKeys::GamepadButtonDpadRight;
        buttonMap[0x00010] = GamepadKeys::GamepadButtonStart;
        buttonMap[0x00020] = GamepadKeys::GamepadButtonSelect;
        buttonMap[0x00040] = GamepadKeys::GamepadButtonLeftThumb;
        buttonMap[0x00080] = GamepadKeys::GamepadButtonRightThumb;
        buttonMap[0x00100] = GamepadKeys::GamepadButtonLeftBumper;
        buttonMap[0x00200] = GamepadKeys::GamepadButtonRightBumper;
        buttonMap[0x00400] = GamepadKeys::GamepadAxisLeftTrigger;
        buttonMap[0x00800] = GamepadKeys::GamepadAxisRightTrigger;
        buttonMap[0x01000] = GamepadKeys::GamepadButtonSouth;
        buttonMap[0x02000] = GamepadKeys::GamepadButtonEast;
        buttonMap[0x04000] = GamepadKeys::GamepadButtonWest;
        buttonMap[0x08000] = GamepadKeys::GamepadButtonNorth;
        buttonMap[0x10000] = GamepadKeys::GamepadButtonHome;

        QueryConnectedDevices();
    }

    void SetPlayerNumberLight(i32 device_id, i32 device_type, i32 player_number, bool is_joycon_pair)
    {
        if (device_type == JS_TYPE_JOYCON_RIGHT && !is_joycon_pair)
        {
            JslSetPlayerNumber(device_id, 0x8 >> (player_number - 1));
        }
        else if (device_type == JS_TYPE_DS)
        {
            i32 player_number_mask = 0;
            switch (player_number)
            {
            case 1:
                player_number_mask = DS5_PLAYER_1;
                break;
            case 2:
                player_number_mask = DS5_PLAYER_2;
                break;
            case 3:
                player_number_mask = DS5_PLAYER_3;
                break;
            case 4:
                player_number_mask = DS5_PLAYER_4;
                break;
            case 5:
                player_number_mask = DS5_PLAYER_5;
                break;
            }
            JslSetPlayerNumber(device_id, player_number_mask);
        }
        else if (device_type == JS_TYPE_DS3)
        {
            i32 playerNumberMask = 0;
            switch (player_number)
            {
            case 1:
                playerNumberMask = DS3_PLAYER_1;
                break;
            case 2:
                playerNumberMask = DS3_PLAYER_2;
                break;
            case 3:
                playerNumberMask = DS3_PLAYER_3;
                break;
            case 4:
                playerNumberMask = DS3_PLAYER_4;
                break;
            }
            JslSetPlayerNumber(device_id, playerNumberMask);
        }
        else
        {
            JslSetPlayerNumber(device_id, player_number);
        }
    }


    void RemoveRegisteredInputDeviceByIndex(i32 index_to_remove)
    {
        i32 device_id = joyShockDevices[index_to_remove].deviceId;
        i32 device_id2 = joyShockDevices[index_to_remove].deviceId2;

        joyShockDevices.erase(joyShockDevices.begin() + index_to_remove);
        connectedDeviceIds.erase(device_id);
        connectedDeviceIds.erase(device_id2);
    }

    void RegisterInputDevice(i32 player_id, i32 device_id, i32 device_type, i32 index_to_join_joycon = -1)
    {
        if (0 <= index_to_join_joycon && index_to_join_joycon < joyShockDevices.size())
        {
            i32 indexToRemove, playerId;
            GetPlayerIdAndIndexByDeviceId(device_id, playerId, indexToRemove);
            joyShockDevices[index_to_join_joycon].deviceId2 = device_id;
            joyShockDevices[index_to_join_joycon].deviceType2 = device_type;
            joyShockDevices[index_to_join_joycon].bIsJoyconPair = true;
            RemoveRegisteredInputDeviceByIndex(indexToRemove);

            JoyShockDeviceInfo device_info;
            device_info.playerId = joyShockDevices[index_to_join_joycon].playerId;
            device_info.bIsJoyconPair = joyShockDevices[index_to_join_joycon].bIsJoyconPair;
            device_info.deviceId = joyShockDevices[index_to_join_joycon].deviceId;
            device_info.deviceType = joyShockDevices[index_to_join_joycon].deviceType;
            device_info.deviceId2 = joyShockDevices[index_to_join_joycon].deviceId2;
            device_info.deviceType2 = joyShockDevices[index_to_join_joycon].deviceType2;

            //OnJoyShockDeviceConnected().Broadcast(deviceInfo);
        }
        else if (connectedDeviceIds.contains(device_id))
        {
            SetPlayerIdByDeviceId(device_id, player_id);
        }
        else
        {
            JoyShockDeviceEntry new_device;
            new_device.playerId = player_id;
            new_device.deviceId = device_id;
            new_device.deviceType = device_type;

            joyShockDevices.push_back(new_device);

            JoyShockDeviceInfo device_info;
            device_info.playerId = new_device.playerId;
            device_info.deviceId = new_device.deviceId;
            device_info.deviceType = new_device.deviceType;

            //OnJoyShockDeviceConnected().Broadcast(device_info);
        }
        SetPlayerNumberLight(device_id, device_type, player_id + 1, index_to_join_joycon >= 0);
        connectedDeviceIds.insert(device_id);
    }

    void RemoveRegisteredInputDeviceByDeviceId(i32 device_id)
    {
        bool found = false;
        i32 i = 0;
        while (i < joyShockDevices.size() && !found)
        {
            found = (joyShockDevices[i].deviceId == device_id) || (joyShockDevices[i].bIsJoyconPair && joyShockDevices[i].deviceId2 == device_id);

            if (found)
            {
                if (joyShockDevices[i].bIsJoyconPair)
                {
                    JoyShockDeviceInfo deviceInfo;
                    deviceInfo.playerId = joyShockDevices[i].playerId;
                    deviceInfo.bIsJoyconPair = joyShockDevices[i].bIsJoyconPair;

                    if (joyShockDevices[i].deviceId == device_id)
                    {
                        deviceInfo.deviceId = joyShockDevices[i].deviceId;
                        deviceInfo.deviceType = joyShockDevices[i].deviceType;

                        joyShockDevices[i].deviceId = joyShockDevices[i].deviceId2;
                        joyShockDevices[i].deviceType = joyShockDevices[i].deviceType2;
                        joyShockDevices[i].lastButtonData = joyShockDevices[i].lastButtonData2;
                    }
                    else
                    {
                        deviceInfo.deviceId2 = joyShockDevices[i].deviceId2;
                        deviceInfo.deviceType2 = joyShockDevices[i].deviceType2;
                    }
                    joyShockDevices[i].lastButtonData2 = 0;
                    joyShockDevices[i].bIsJoyconPair = false;
                    if (joyShockDevices[i].deviceType == JS_TYPE_JOYCON_RIGHT)
                    {
                        SetPlayerNumberLight(joyShockDevices[i].deviceId, joyShockDevices[i].deviceType, joyShockDevices[i].playerId + 1, joyShockDevices[i].bIsJoyconPair);
                    }
                    //OnJoyShockDeviceDisconnected().Broadcast(deviceInfo);
                }
                else
                {
                    JoyShockDeviceInfo deviceInfo;
                    deviceInfo.playerId = joyShockDevices[i].playerId;
                    deviceInfo.deviceId = joyShockDevices[i].deviceId;
                    deviceInfo.deviceType = joyShockDevices[i].deviceType;
                    //OnJoyShockDeviceDisconnected().Broadcast(deviceInfo);
                    joyShockDevices.erase(joyShockDevices.begin() + i);
                }
                connectedDeviceIds.erase(device_id);
            }
            ++i;
        }
    }

    bool JoinJoyconsByDeviceId(i32 device_id, i32 device_id_2)
    {
        bool is_valid = false;
        i32 index = -1;
        i32 index_2 = -1;
        i32 i = 0;
        while (i < joyShockDevices.size() && (index == -1 || index_2 == -1))
        {
            if (joyShockDevices[i].deviceId == device_id)
            {
                index = i;
            }
            else if (joyShockDevices[i].deviceId == device_id_2)
            {
                index_2 = i;
            }

            if (index != -1 && index_2 != -1)
            {
                i32 device_type = joyShockDevices[index].deviceType;
                i32 device_type_2 = joyShockDevices[index_2].deviceType;
                is_valid = (device_type == JS_TYPE_JOYCON_LEFT && device_type_2 == JS_TYPE_JOYCON_RIGHT) ||
                    (device_type == JS_TYPE_JOYCON_RIGHT && device_type_2 == JS_TYPE_JOYCON_LEFT) && !joyShockDevices[index].bIsJoyconPair && !joyShockDevices[index_2].bIsJoyconPair;
                if (is_valid)
                {
                    RegisterInputDevice(joyShockDevices[index].playerId, device_id_2, device_type_2, index);
                    SetPlayerNumberLight(joyShockDevices[index].deviceId, joyShockDevices[index].deviceType, joyShockDevices[index].playerId + 1, joyShockDevices[index].bIsJoyconPair);
                }
            }
            ++i;
        }

        return is_valid;
    }

    bool QueryConnectedDevices()
    {
        i32 old_connected_devices = connectedDevices;
        connectedDevices = JslConnectDevices();

        if (connectedDevices != old_connected_devices)
        {
            if (connectedDevices == 0)
            {
                joyShockDevices.clear();
                connectedDeviceIds.clear();
                return 0;
            }

            Array<i32> device_ids;
            device_ids.resize(connectedDevices);
            JslGetConnectedDeviceHandles(device_ids.data(), connectedDevices);

            for (i32 i = 0; i < device_ids.size(); ++i)
            {
                RegisterInputDevice(i, device_ids[i], JslGetControllerType(device_ids[i]));
                JslSetAutomaticCalibration(device_ids[i], true);
            }
            UnorderedSet<i32> devices_to_remove;
            for (i32 device_id : connectedDeviceIds)
            {
                if (std::find(device_ids.begin(), device_ids.end(), device_id) == device_ids.end())
                {
                    devices_to_remove.insert(device_id);
                }
            }
            for (i32 deviceId : devices_to_remove)
            {
                RemoveRegisteredInputDeviceByDeviceId(deviceId);
            }
            JoinJoyconsByDeviceId(0, 1);

            return true;
        }
        return false;
    }


    void GetPlayerIdAndIndexByDeviceId(i32 device_id, i32& player_id, i32& index)
    {
        bool found = false;
        i32 i = 0;
        player_id = -1;
        index = -1;

        while (i < joyShockDevices.size() && !found)
        {
            found = joyShockDevices[i].deviceId == device_id;

            if (found)
            {
                player_id = joyShockDevices[i].playerId;
                index = i;
            }
            ++i;
        }
    }

    bool SetPlayerIdByDeviceId(i32 device_id, i32 player_id)
    {
        bool found = false;
        i32 i = 0;

        while (i < joyShockDevices.size() && !found)
        {
            found = joyShockDevices[i].deviceId == device_id || joyShockDevices[i].deviceId2 == device_id;

            if (found)
            {
                joyShockDevices[i].playerId = player_id;
            }
            ++i;
        }

        return found;
    }

    ListenerAction start()
    {
        init_joyshock();
        input_registry->input_actions = asset_get_pool<InputAction>();

        return ListenerAction::StayListening;
    }

    ListenerAction update()
    {
        // -------------------------
        // GAMEPAD STATE
        // -------------------------
        for (i32 i = 0; i < joyShockDevices.size(); ++i)
        {
            handle_joyshock_controller_events(joyShockDevices[i].playerId, joyShockDevices[i].deviceId, joyShockDevices[i].deviceType, joyShockDevices[i].bIsJoyconPair, joyShockDevices[i].lastButtonData);
            if (joyShockDevices[i].bIsJoyconPair)
            {
                handle_joyshock_controller_events(joyShockDevices[i].playerId, joyShockDevices[i].deviceId2, joyShockDevices[i].deviceType2, joyShockDevices[i].bIsJoyconPair, joyShockDevices[i].lastButtonData2);
            }
        }
        if (!input_registry->input_actions) return ListenerAction::StayListening;

        // -------------------------
        // INPUT ACTIONS
        // -------------------------

        InputAction* input_action_pool = (InputAction*)(input_registry->input_actions->data_pool.elements);
        for (u32 i = 0; i < input_registry->input_actions->data_pool.sparse_set.count; ++i)
        {
            if (!&input_action_pool[i]) continue;

            if (input_action_pool[i].m_InputType == InputType::Digital)
            {
                InputContent button_state;
                GetButtonState(input_action_pool[i].m_Key, button_state);
                if (!button_state.isDirty) continue;
                button_state.isDirty = false;

                if (button_state.currentDigitalValue != button_state.lastDigitalValue)
                {
                    if (button_state.currentDigitalValue)
                    {
                        on_controller_button_pressed(&input_action_pool[i], false);
                    }
                    else
                    {
                        on_controller_button_released(&input_action_pool[i], false);
                    }
                }
                else if (button_state.currentDigitalValue)
                {
                    on_controller_button_pressed(&input_action_pool[i], true);
                }
            }
            else if (input_action_pool[i].m_InputType == InputType::Axis1D)
            {
                InputContent button_state;
                GetButtonState(input_action_pool[i].m_Key, button_state);
                if (!button_state.isDirty) continue;
                button_state.isDirty = false;

                on_controller_analog(&input_action_pool[i], button_state.currentAnalogValue);
            }
            else if (input_action_pool[i].m_InputType == InputType::Axis2D)
            {
                InputContent button_state_x, button_state_y;
                GetButtonState(input_action_pool[i].m_PairedInputKeyX, button_state_x);
                GetButtonState(input_action_pool[i].m_PairedInputKeyY, button_state_y);
                if (!button_state_x.isDirty && !button_state_y.isDirty) continue;
                button_state_x.isDirty = false;
                button_state_y.isDirty = false;

                on_controller_vector2(&input_action_pool[i], Vector2(button_state_x.currentAnalogValue, button_state_y.currentAnalogValue));
            }
            else if (input_action_pool[i].m_InputType == InputType::Axis3D)
            {
                InputContent button_state_x, button_state_y, button_state_z;
                GetButtonState(input_action_pool[i].m_PairedInputKeyX, button_state_x);
                GetButtonState(input_action_pool[i].m_PairedInputKeyY, button_state_y);
                GetButtonState(input_action_pool[i].m_PairedInputKeyZ, button_state_z);
                if (!button_state_x.isDirty && !button_state_y.isDirty && !button_state_z.isDirty) continue;
                button_state_x.isDirty = false;
                button_state_y.isDirty = false;
                button_state_z.isDirty = false;

                on_controller_vector3(&input_action_pool[i], Vector3(button_state_x.currentAnalogValue, button_state_y.currentAnalogValue, button_state_z.currentAnalogValue));
            }
            else if (input_action_pool[i].m_InputType == InputType::Axis4D)
            {
                InputContent button_state_x, button_state_y, button_state_z, button_state_w;
                GetButtonState(input_action_pool[i].m_PairedInputKeyX, button_state_x);
                GetButtonState(input_action_pool[i].m_PairedInputKeyY, button_state_y);
                GetButtonState(input_action_pool[i].m_PairedInputKeyZ, button_state_z);
                GetButtonState(input_action_pool[i].m_PairedInputKeyW, button_state_w);
                if (!button_state_x.isDirty && !button_state_y.isDirty && !button_state_z.isDirty && !button_state_w.isDirty) continue;
                button_state_x.isDirty = false;
                button_state_y.isDirty = false;
                button_state_z.isDirty = false;
                button_state_w.isDirty = false;

                on_controller_vector4(&input_action_pool[i], Vector4(button_state_x.currentAnalogValue,
                    button_state_y.currentAnalogValue,
                    button_state_z.currentAnalogValue,
                    button_state_w.currentAnalogValue));
            }
        }

        // -------------------------
        // INPUT MODIFIERS
        // -------------------------

        for (u32 i = 0; i < input_registry->next_input_modifier_type_index; ++i)
        {
            InputModifierPool& input_modifier_data = input_registry->input_modifier_pool[i];
            if(input_modifier_data.type_index == 0) continue;
            // for (u32 j = 0; j < input_modifier_data.data_pool.sparse_set.count; ++j)
            // {
            //     GamepadKeys action_key = input_modifier_data.modifier_info[j].action_key;
            //     void* data = pool_get_raw_data(&input_modifier_data.data_pool, j);
            //
            //     if (!input_action_context_map.contains(action_key)) continue;
            //
            //     InputActionContext context = input_action_context_map[action_key];
            //
            //     input_modifier_data.fn_invoke_modify(data, context.inputValue, context.inputType);
            //
            //     input_action_context_map[action_key] = context;
            // }
        }

        // -------------------------
        // INPUT ACTIONS
        // -------------------------
        input_action_pool = (InputAction*)(input_registry->input_actions->data_pool.elements);
        for (u32 i = 0; i < input_registry->input_actions->data_pool.sparse_set.count; ++i)
        {
            if (!&input_action_pool[i]) continue;

            InputActionContext context;
            get_input_action_context(input_action_pool[i].m_Key, context);
            event_broadcast<const InputActionContext&>(input_action_pool[i].input_performed_event, context);
        }

        return ListenerAction::StayListening;
    }

    void on_controller_analog(InputAction* input_action, f32 analog_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Axis1D;
        input_context.inputValue = Vector4(analog_value, 0.f, 0.f, 0.f);

        input_context.controllerId = 1;
    }

    void on_controller_vector2(InputAction* input_action, const Vector2& vector2_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Axis2D;
        input_context.inputValue = Vector4(vector2_value.x, vector2_value.y, 0.f, 0.f);

        input_context.controllerId = 1;
    }

    void on_controller_vector3(InputAction* input_action, const Vector3& vector3_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Axis3D;
        input_context.inputValue = Vector4(vector3_value.x, vector3_value.y, vector3_value.z, 0.f);

        input_context.controllerId = 1;
    }

    void on_controller_vector4(InputAction* input_action, const Vector4& vector4_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Axis4D;
        input_context.inputValue = Vector4(vector4_value.x, vector4_value.y, vector4_value.z, vector4_value.w);

        input_context.controllerId = 1;
    }

    void on_controller_button_pressed(InputAction* input_action, bool is_repeat)
    {
        if ((input_action->m_TriggerType == TriggerType::Pressed && is_repeat) || input_action->m_TriggerType == TriggerType::Released) return;

        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Digital;
        input_context.inputValue = Vector4(1.f, 0.f, 0.f, 0.f);

        input_context.bIsPressed = true;
        input_context.bIsRepeat = is_repeat;
        input_context.controllerId = 1;
    }

    void on_controller_button_released(InputAction* input_action, bool is_repeat)
    {
        if (input_action->m_TriggerType == TriggerType::Pressed || input_action->m_TriggerType == TriggerType::Down) return;

        InputActionContext& input_context = input_action_context_map[input_action->m_Key];
        input_context.inputType = InputType::Digital;
        input_context.inputValue = Vector4(0.f, 0.f, 0.f, 0.f);

        input_context.bIsPressed = false;
        input_context.bIsRepeat = is_repeat;
        input_context.controllerId = 1;
    }

    void handle_button_press(bool pressed, GamepadKeys key, i32 player_id)
    {
        InputContent& input_content = controller_state[key];
        input_content.lastDigitalValue = input_content.currentDigitalValue;
        input_content.currentDigitalValue = pressed;
        input_content.currentAnalogValue = input_content.currentDigitalValue;
        input_content.isDirty = true;
    }

    void handle_axis(f32 value, GamepadKeys key, i32 player_id)
    {
        InputContent& input_content = controller_state[key];
        input_content.lastDigitalValue = input_content.currentDigitalValue;
        input_content.currentDigitalValue = value > g_LowerDeadzone;
        input_content.currentAnalogValue = value;
        input_content.isDirty = true;
    }

    void handle_joyshock_controller_events(i32 player_id, i32 device_id, i32 device_type, bool is_joycon_pair, i32& last_button_data)
    {
        JOY_SHOCK_STATE simpleState = JslGetSimpleState(device_id);

        i32 buttonMask = 0x00001;
        if (is_joycon_pair || (device_type != JS_TYPE_JOYCON_LEFT && device_type != JS_TYPE_JOYCON_RIGHT))
        {
            for (i32 i = 0; i < NUM_BUTTONS; ++i)
            {
                handle_button_press(buttonMask & simpleState.buttons, buttonMap[buttonMask], player_id);
                buttonMask = buttonMask << 1;
            }
        }
        switch (device_type)
        {
        case JS_TYPE_JOYCON_LEFT:
            if (!is_joycon_pair)
            {
                handle_button_press(0x00008 & simpleState.buttons, GamepadKeys::GamepadButtonNorth, player_id);
                handle_button_press(0x00004 & simpleState.buttons, GamepadKeys::GamepadButtonSouth, player_id);
                handle_button_press(0x00001 & simpleState.buttons, GamepadKeys::GamepadButtonWest, player_id);
                handle_button_press(0x00002 & simpleState.buttons, GamepadKeys::GamepadButtonEast, player_id);
                handle_button_press(0x00040 & simpleState.buttons, GamepadKeys::GamepadButtonLeftThumb, player_id);
                handle_button_press(0x00100 & simpleState.buttons, GamepadKeys::GamepadButtonLeftBumper, player_id);
                handle_button_press(0x00400 & simpleState.buttons, GamepadKeys::GamepadAxisLeftTrigger, player_id);
                handle_button_press(0x00020 & simpleState.buttons, GamepadKeys::GamepadButtonStart, player_id);
                handle_button_press(0x400000 & simpleState.buttons, GamepadKeys::SwitchButtonSL, player_id);
                handle_button_press(0x800000 & simpleState.buttons, GamepadKeys::SwitchButtonSR, player_id);
            }
            else
            {
                handle_button_press(0x400000 & simpleState.buttons, GamepadKeys::SwitchButtonLeftSL, player_id);
                handle_button_press(0x800000 & simpleState.buttons, GamepadKeys::SwitchButtonLeftSR, player_id);
            }

            handle_button_press(0x200000 & simpleState.buttons, GamepadKeys::SwitchButtonCapture, player_id);
            break;
        case JS_TYPE_JOYCON_RIGHT:
            if (!is_joycon_pair)
            {
                handle_button_press(0x04000 & simpleState.buttons, GamepadKeys::GamepadButtonNorth, player_id);
                handle_button_press(0x02000 & simpleState.buttons, GamepadKeys::GamepadButtonSouth, player_id);
                handle_button_press(0x01000 & simpleState.buttons, GamepadKeys::GamepadButtonWest, player_id);
                handle_button_press(0x08000 & simpleState.buttons, GamepadKeys::GamepadButtonEast, player_id);
                handle_button_press(0x00080 & simpleState.buttons, GamepadKeys::GamepadButtonLeftThumb, player_id);
                handle_button_press(0x00200 & simpleState.buttons, GamepadKeys::GamepadButtonLeftBumper, player_id);
                handle_button_press(0x00800 & simpleState.buttons, GamepadKeys::GamepadAxisLeftTrigger, player_id);
                handle_button_press(0x00010 & simpleState.buttons, GamepadKeys::GamepadButtonStart, player_id);
                handle_button_press(0x400000 & simpleState.buttons, GamepadKeys::SwitchButtonSL, player_id);
                handle_button_press(0x800000 & simpleState.buttons, GamepadKeys::SwitchButtonSR, player_id);
            }
            else
            {
                handle_button_press(0x400000 & simpleState.buttons, GamepadKeys::SwitchButtonRightSL, player_id);
                handle_button_press(0x800000 & simpleState.buttons, GamepadKeys::SwitchButtonRightSR, player_id);
            }

            break;
        case JS_TYPE_PRO_CONTROLLER:
            handle_button_press(0x200000 & simpleState.buttons, GamepadKeys::SwitchButtonCapture, player_id);
            break;
        case JS_TYPE_DS4:
            handle_button_press(0x200000 & simpleState.buttons, GamepadKeys::DualShockButtonTouchpadDown, player_id);
            break;
        case JS_TYPE_DS:
            handle_button_press(0x200000 & simpleState.buttons, GamepadKeys::DualShockButtonCreate, player_id);
            handle_button_press(0x400000 & simpleState.buttons, GamepadKeys::DualShockButtonMic, player_id);
            break;
        }
        if (!is_joycon_pair && (device_type == JS_TYPE_JOYCON_LEFT || device_type == JS_TYPE_JOYCON_RIGHT))
        {
            if (device_type == JS_TYPE_JOYCON_LEFT)
            {
                const f32 AuxStickX = simpleState.stickLX;
                simpleState.stickLX = -simpleState.stickLY;
                simpleState.stickLY = AuxStickX;
            }
            if (device_type == JS_TYPE_JOYCON_RIGHT)
            {
                simpleState.stickLX = simpleState.stickRY;
                simpleState.stickLY = -simpleState.stickRX;

                simpleState.stickRX = 0.f;
                simpleState.stickRY = 0.f;
            }
        }
        if (device_type != JS_TYPE_JOYCON_RIGHT || !is_joycon_pair)
        {
            handle_axis(simpleState.stickLX, GamepadKeys::GamepadAxisLeftX, player_id);
            handle_axis(simpleState.stickLY, GamepadKeys::GamepadAxisLeftY, player_id);
            handle_axis(simpleState.lTrigger, GamepadKeys::GamepadAxisLeftTrigger, player_id);
        }
        if (device_type != JS_TYPE_JOYCON_LEFT)
        {
            handle_axis(simpleState.stickRX, GamepadKeys::GamepadAxisRightX, player_id);
            handle_axis(simpleState.stickRY, GamepadKeys::GamepadAxisRightY, player_id);
            handle_axis(simpleState.rTrigger, GamepadKeys::GamepadAxisRightTrigger, player_id);
        }
        if (device_type == JS_TYPE_DS || device_type == JS_TYPE_DS4)
        {
            TOUCH_STATE touchState = JslGetTouchState(device_id);

            handle_button_press(touchState.t0Down, GamepadKeys::DualShockButtonFirstTouchpad, player_id);
            handle_button_press(touchState.t1Down, GamepadKeys::DualShockButtonSecondTouchpad, player_id);
            if (touchState.t0Down)
            {
                handle_axis(touchState.t0X, GamepadKeys::DualShockAxisFirstTouchpadX, player_id);
                handle_axis(touchState.t0Y, GamepadKeys::DualShockAxisFirstTouchpadY, player_id);
            }
            if (touchState.t1Down)
            {
                handle_axis(touchState.t1X, GamepadKeys::DualShockAxisSecondTouchpadX, player_id);
                handle_axis(touchState.t1Y, GamepadKeys::DualShockAxisSecondTouchpadY, player_id);
            }
        }
        if (device_type == JS_TYPE_DS3)
        {
            DS3_STATE ds3State = JslGetDS3State(device_id);

            handle_axis(ds3State.dpadUp, GamepadKeys::DualShockAxisDpadUp, player_id);
            handle_axis(ds3State.dpadRight, GamepadKeys::DualShockAxisDpadRight, player_id);
            handle_axis(ds3State.dpadDown, GamepadKeys::DualShockAxisDpadDown, player_id);
            handle_axis(ds3State.dpadLeft, GamepadKeys::DualShockAxisDpadLeft, player_id);
            handle_axis(ds3State.cross, GamepadKeys::DualShockAxisSouth, player_id);
            handle_axis(ds3State.circle, GamepadKeys::DualShockAxisEast, player_id);
            handle_axis(ds3State.square, GamepadKeys::DualShockAxisWest, player_id);
            handle_axis(ds3State.triangle, GamepadKeys::DualShockAxisNorth, player_id);
            handle_axis(ds3State.l1, GamepadKeys::DualShockAxisLeftBumper, player_id);
            handle_axis(ds3State.r1, GamepadKeys::DualShockAxisRightBumper, player_id);
        }

        IMU_STATE imuState = JslGetIMUState(device_id);
        MOTION_STATE motionState = JslGetMotionState(device_id);

        if (is_joycon_pair && device_type == JS_TYPE_JOYCON_LEFT)
        {
            handle_axis(imuState.gyroX, GamepadKeys::SwitchAxisLeftGyroX, player_id);
            handle_axis(imuState.gyroY, GamepadKeys::SwitchAxisLeftGyroY, player_id);
            handle_axis(imuState.gyroZ, GamepadKeys::SwitchAxisLeftGyroZ, player_id);

            handle_axis(motionState.accelX, GamepadKeys::SwitchAxisLeftAccelX, player_id);
            handle_axis(motionState.accelY, GamepadKeys::SwitchAxisLeftAccelY, player_id);
            handle_axis(motionState.accelZ, GamepadKeys::SwitchAxisLeftAccelZ, player_id);

            handle_axis(motionState.gravX, GamepadKeys::SwitchAxisLeftGravX, player_id);
            handle_axis(motionState.gravY, GamepadKeys::SwitchAxisLeftGravY, player_id);
            handle_axis(motionState.gravZ, GamepadKeys::SwitchAxisLeftGravZ, player_id);

            handle_axis(motionState.quatX, GamepadKeys::SwitchAxisLeftQuatX, player_id);
            handle_axis(motionState.quatY, GamepadKeys::SwitchAxisLeftQuatY, player_id);
            handle_axis(motionState.quatZ, GamepadKeys::SwitchAxisLeftQuatZ, player_id);
            handle_axis(motionState.quatW, GamepadKeys::SwitchAxisLeftQuatW, player_id);
        }
        else
        {
            /*
            if (!is_joycon_pair)
            {
                if (device_type == JS_TYPE_JOYCON_LEFT)
                {
                    float AuxOrientationX = orientation.x;
                    orientation.x = orientation.y;
                    orientation.y = -AuxOrientationX;
                }
                else if (device_type == JS_TYPE_JOYCON_RIGHT)
                {
                    float AuxOrientationX = orientation.y;
                    orientation.x = -orientation.y;
                    orientation.x = AuxOrientationX;
                }

            }
            */

            handle_axis(imuState.gyroX, GamepadKeys::GamepadAxisGyroX, player_id);
            handle_axis(imuState.gyroY, GamepadKeys::GamepadAxisGyroY, player_id);
            handle_axis(imuState.gyroZ, GamepadKeys::GamepadAxisGyroZ, player_id);
            handle_axis(imuState.accelX, GamepadKeys::GamepadAxisTotalAccelX, player_id);
            handle_axis(imuState.accelY, GamepadKeys::GamepadAxisTotalAccelY, player_id);
            handle_axis(imuState.accelZ, GamepadKeys::GamepadAxisTotalAccelZ, player_id);
            handle_axis(motionState.accelX, GamepadKeys::GamepadAxisAccelX, player_id);
            handle_axis(motionState.accelY, GamepadKeys::GamepadAxisAccelY, player_id);
            handle_axis(motionState.accelZ, GamepadKeys::GamepadAxisAccelZ, player_id);
            handle_axis(motionState.gravX, GamepadKeys::GamepadAxisGravX, player_id);
            handle_axis(motionState.gravY, GamepadKeys::GamepadAxisGravY, player_id);
            handle_axis(motionState.gravZ, GamepadKeys::GamepadAxisGravZ, player_id);
            handle_axis(motionState.quatX, GamepadKeys::GamepadAxisQuatX, player_id);
            handle_axis(motionState.quatY, GamepadKeys::GamepadAxisQuatY, player_id);
            handle_axis(motionState.quatZ, GamepadKeys::GamepadAxisQuatZ, player_id);
            handle_axis(motionState.quatW, GamepadKeys::GamepadAxisQuatW, player_id);
        }

        last_button_data = simpleState.buttons;
    }
}