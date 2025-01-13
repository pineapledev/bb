#include "input_registry.h"
#include "core/engine.h"
#include "input_action.h"
#include "core/asset.h"
#include "JoyShockLibrary.h"
#include "glfw/glfw3.h"

namespace nit
{
#define NIT_CHECK_INPUT_REGISTRY_CREATED NIT_CHECK(input_registry)

    InputRegistry* input_registry = nullptr;


    // ------------------------
    // INPUT 
    // ------------------------

    Set<i32> player_ids;
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
    // XBOX 
    // ------------------------

    Array<GLFWDeviceInfo> glfw_devices;


    // ------------------------
    // JOYSHOCK 
    // ------------------------

    i32 last_assigned_input_handle;
    Map<i32, GamepadKeys> button_map;
    Array<JoyShockDeviceEntry> joyshock_devices;
    UnorderedSet<i32> connected_joyshock_device_ids;
    i32 connected_joyshock_devices = 0;

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

        for (i32 i = 0; i < NIT_MAX_PLAYERS; ++i)
        {
            player_ids.insert(i);
        }

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

        for (u32 i = 0; i < input_registry->next_input_modifier_type_index - 1; ++i)
        {
            InputModifierPool& data = input_registry->input_modifier_pool[i];
            pool_free(&data.data_pool);
        }
    }

    InputModifierPool* input_find_modifier_pool(const Type* type)
    {
        NIT_CHECK_INPUT_REGISTRY_CREATED

        for (u32 i = 0; i < input_registry->next_input_modifier_type_index - 1; ++i)
        {
            InputModifierPool& input_modifier_pool = input_registry->input_modifier_pool[i];
            if (input_modifier_pool.data_pool.type == type)
            {
                return &input_modifier_pool;
            }
        }
        return nullptr;
    }

    bool input_enabled = true;

    ControllerColor int_to_controller_color(i32 body_color, i32 left_button_color, i32 right_button_color, i32 left_grip_color, i32 right_grip_color)
    {
        ControllerColor controller_color;

        controller_color.body_color.x = static_cast<f32>((body_color >> 16) & 0xFF);
        controller_color.body_color.y = static_cast<f32>((body_color >> 8) & 0xFF);
        controller_color.body_color.z = static_cast<f32>(body_color & 0xFF);

        controller_color.left_button_color.x = static_cast<f32>((left_button_color >> 16) & 0xFF);
        controller_color.left_button_color.y = static_cast<f32>((left_button_color >> 8) & 0xFF);
        controller_color.left_button_color.z = static_cast<f32>(left_button_color & 0xFF);

        controller_color.right_button_color.x = static_cast<f32>((right_button_color >> 16) & 0xFF);
        controller_color.right_button_color.y = static_cast<f32>((right_button_color >> 8) & 0xFF);
        controller_color.right_button_color.z = static_cast<f32>(right_button_color & 0xFF);

        controller_color.left_grip_color.x = static_cast<f32>((left_grip_color >> 16) & 0xFF);
        controller_color.left_grip_color.y = static_cast<f32>((left_grip_color >> 8) & 0xFF);
        controller_color.left_grip_color.z = static_cast<f32>(left_grip_color & 0xFF);

        controller_color.right_grip_color.x = static_cast<f32>((right_grip_color >> 16) & 0xFF);
        controller_color.right_grip_color.y = static_cast<f32>((right_grip_color >> 8) & 0xFF);
        controller_color.right_grip_color.z = static_cast<f32>(right_grip_color & 0xFF);

        return controller_color;
    }

    ControllerColor get_controller_colors_by_player_id(i32 player_id)
    {
        i32 device_id, device_id_2, index;
        ControllerColor controller_color;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return controller_color;

        i32 body_color = 0xFFFFFF, left_button_color = 0xFFFFFF, right_button_color = 0xFFFFFF, left_grip_color = 0xFFFFFF, right_grip_color = 0xFFFFFF;
        if (joyshock_devices[index].device_type == JS_TYPE_JOYCON_LEFT)
        {
            left_grip_color = JslGetControllerBodyColour(device_id);
            left_button_color = JslGetControllerButtonColour(device_id);
            right_grip_color = JslGetControllerBodyColour(device_id_2);
            right_button_color = JslGetControllerButtonColour(device_id_2);
        }
        else if (joyshock_devices[index].device_type == JS_TYPE_JOYCON_RIGHT)
        {
            left_grip_color = JslGetControllerBodyColour(device_id_2);
            left_button_color = JslGetControllerButtonColour(device_id_2);
            right_grip_color = JslGetControllerBodyColour(device_id);
            right_button_color = JslGetControllerButtonColour(device_id);
        }
        else
        {
            body_color = JslGetControllerBodyColour(device_id);
            left_button_color = JslGetControllerButtonColour(device_id);
            right_button_color = left_button_color;
            left_grip_color = JslGetControllerLeftGripColour(device_id);
            right_grip_color = JslGetControllerRightGripColour(device_id);
        }
        return int_to_controller_color(body_color, left_button_color, right_button_color, left_grip_color, right_grip_color);
    }

    ControllerColor get_controller_colors_by_device_id(i32 device_id)
    {
        i32 body_color = 0xFFFFFF, left_button_color = 0xFFFFFF, right_button_color = 0xFFFFFF, left_grip_color = 0xFFFFFF, right_grip_color = 0xFFFFFF;
        if (JslGetControllerType(device_id) == JS_TYPE_JOYCON_LEFT)
        {
            left_grip_color = JslGetControllerBodyColour(device_id);
            left_button_color = JslGetControllerButtonColour(device_id);
        }
        else if (JslGetControllerType(device_id) == JS_TYPE_JOYCON_RIGHT)
        {
            right_grip_color = JslGetControllerBodyColour(device_id);
            right_button_color = JslGetControllerButtonColour(device_id);
        }
        else
        {
            body_color = JslGetControllerBodyColour(device_id);
            left_button_color = JslGetControllerButtonColour(device_id);
            right_button_color = left_button_color;
            left_grip_color = JslGetControllerLeftGripColour(device_id);
            right_grip_color = JslGetControllerRightGripColour(device_id);
        }

        return int_to_controller_color(body_color, left_button_color, right_button_color, left_grip_color, right_grip_color);
    }

    void set_controller_light_color_by_player_id(i32 controller_id, const Vector3& color)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(controller_id, device_id, device_id_2, index);
        if (index < 0) return;

        set_controller_light_color_by_device_id(device_id, color);
    }

    void set_controller_light_color_by_device_id(i32 device_id, const Vector3& color)
    {
        i32 light_color, color_r = static_cast<i32>(color.x), color_g = static_cast<i32>(color.y), color_b = static_cast<i32>(color.z);

        light_color = (color_r << 16) & 0x00ff0000;
        light_color += (color_g << 8) & 0x0000ff00;
        light_color += (color_b) & 0x000000ff;

        JslSetLightColour(device_id, light_color);
    }

    bool is_key_pressed(u32 key)
    {
        if (!input_enabled) return false;
        const auto state = input_get_key(key);
        return state == GLFW_PRESS;
    }

    void get_button_state(GamepadKeys key, InputContent& input_content)
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

    void set_input_enabled(const bool consumed)
    {
        input_enabled = consumed;
    }

    bool is_input_enabled()
    {
        return input_enabled;
    }

    void init_joyshock()
    {
        button_map[0x00001] = GamepadKeys::GamepadButtonDpadUp;
        button_map[0x00002] = GamepadKeys::GamepadButtonDpadDown;
        button_map[0x00004] = GamepadKeys::GamepadButtonDpadLeft;
        button_map[0x00008] = GamepadKeys::GamepadButtonDpadRight;
        button_map[0x00010] = GamepadKeys::GamepadButtonStart;
        button_map[0x00020] = GamepadKeys::GamepadButtonSelect;
        button_map[0x00040] = GamepadKeys::GamepadButtonLeftThumb;
        button_map[0x00080] = GamepadKeys::GamepadButtonRightThumb;
        button_map[0x00100] = GamepadKeys::GamepadButtonLeftBumper;
        button_map[0x00200] = GamepadKeys::GamepadButtonRightBumper;
        button_map[0x00400] = GamepadKeys::GamepadAxisLeftTrigger;
        button_map[0x00800] = GamepadKeys::GamepadAxisRightTrigger;
        button_map[0x01000] = GamepadKeys::GamepadButtonSouth;
        button_map[0x02000] = GamepadKeys::GamepadButtonEast;
        button_map[0x04000] = GamepadKeys::GamepadButtonWest;
        button_map[0x08000] = GamepadKeys::GamepadButtonNorth;
        button_map[0x10000] = GamepadKeys::GamepadButtonHome;

        query_connected_devices();
    }

    void set_player_number_light(i32 device_id, i32 device_type, i32 player_number, bool is_joycon_pair)
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
            i32 player_number_mask = 0;
            switch (player_number)
            {
            case 1:
                player_number_mask = DS3_PLAYER_1;
                break;
            case 2:
                player_number_mask = DS3_PLAYER_2;
                break;
            case 3:
                player_number_mask = DS3_PLAYER_3;
                break;
            case 4:
                player_number_mask = DS3_PLAYER_4;
                break;
            }
            JslSetPlayerNumber(device_id, player_number_mask);
        }
        else
        {
            JslSetPlayerNumber(device_id, player_number);
        }
    }


    void remove_registered_input_device_by_index(i32 index_to_remove)
    {
        i32 device_id = joyshock_devices[index_to_remove].device_id;
        i32 device_id_2 = joyshock_devices[index_to_remove].device_id_2;

        joyshock_devices.erase(joyshock_devices.begin() + index_to_remove);
        connected_joyshock_device_ids.erase(device_id);
        connected_joyshock_device_ids.erase(device_id_2);
    }

    void register_input_device(i32 player_id, i32 device_id, i32 device_type, i32 index_to_join_joycon = -1)
    {
        if (0 <= index_to_join_joycon && index_to_join_joycon < joyshock_devices.size())
        {
            i32 index_to_remove, other_player_id;
            get_player_id_and_index_by_device_id(device_id, other_player_id, index_to_remove);
            joyshock_devices[index_to_join_joycon].device_id_2 = device_id;
            joyshock_devices[index_to_join_joycon].device_type_2 = device_type;
            joyshock_devices[index_to_join_joycon].is_joycon_pair = true;
            remove_registered_input_device_by_index(index_to_remove);

            JoyShockDeviceInfo device_info;
            device_info.player_id = joyshock_devices[index_to_join_joycon].player_id;
            device_info.is_joycon_pair = joyshock_devices[index_to_join_joycon].is_joycon_pair;
            device_info.device_id = joyshock_devices[index_to_join_joycon].device_id;
            device_info.device_type = joyshock_devices[index_to_join_joycon].device_type;
            device_info.device_id_2 = joyshock_devices[index_to_join_joycon].device_id_2;
            device_info.device_type_2 = joyshock_devices[index_to_join_joycon].device_type_2;

            //OnJoyShockDeviceConnected().Broadcast(deviceInfo);
        }
        else if (connected_joyshock_device_ids.contains(device_id))
        {
            set_player_id_by_device_id(device_id, player_id);
        }
        else
        {
            JoyShockDeviceEntry new_device;
            new_device.player_id = player_id;
            new_device.device_id = device_id;
            new_device.device_type = device_type;

            joyshock_devices.push_back(new_device);

            JoyShockDeviceInfo device_info;
            device_info.player_id = new_device.player_id;
            device_info.device_id = new_device.device_id;
            device_info.device_type = new_device.device_type;

            //OnJoyShockDeviceConnected().Broadcast(device_info);
        }
        set_player_number_light(device_id, device_type, player_id + 1, index_to_join_joycon >= 0);
        connected_joyshock_device_ids.insert(device_id);
    }

    void remove_registered_input_device_by_device_id(i32 device_id)
    {
        bool found = false;
        i32 i = 0;
        while (i < joyshock_devices.size() && !found)
        {
            found = (joyshock_devices[i].device_id == device_id) || (joyshock_devices[i].is_joycon_pair && joyshock_devices[i].device_id_2 == device_id);

            if (found)
            {
                if (joyshock_devices[i].is_joycon_pair)
                {
                    JoyShockDeviceInfo device_info;
                    device_info.player_id = joyshock_devices[i].player_id;
                    device_info.is_joycon_pair = joyshock_devices[i].is_joycon_pair;

                    if (joyshock_devices[i].device_id == device_id)
                    {
                        device_info.device_id = joyshock_devices[i].device_id;
                        device_info.device_type = joyshock_devices[i].device_type;

                        joyshock_devices[i].device_id = joyshock_devices[i].device_id_2;
                        joyshock_devices[i].device_type = joyshock_devices[i].device_type_2;
                        joyshock_devices[i].last_button_data = joyshock_devices[i].last_button_data_2;
                    }
                    else
                    {
                        device_info.device_id_2 = joyshock_devices[i].device_id_2;
                        device_info.device_type_2 = joyshock_devices[i].device_type_2;
                    }
                    joyshock_devices[i].last_button_data_2 = 0;
                    joyshock_devices[i].is_joycon_pair = false;
                    if (joyshock_devices[i].device_type == JS_TYPE_JOYCON_RIGHT)
                    {
                        set_player_number_light(joyshock_devices[i].device_id, joyshock_devices[i].device_type, joyshock_devices[i].player_id + 1, joyshock_devices[i].is_joycon_pair);
                    }
                    //OnJoyShockDeviceDisconnected().Broadcast(deviceInfo);
                }
                else
                {
                    JoyShockDeviceInfo device_info;
                    device_info.player_id = joyshock_devices[i].player_id;
                    device_info.device_id = joyshock_devices[i].device_id;
                    device_info.device_type = joyshock_devices[i].device_type;
                    //OnJoyShockDeviceDisconnected().Broadcast(deviceInfo);
                    joyshock_devices.erase(joyshock_devices.begin() + i);
                    player_ids.insert(device_info.player_id);
                }

                connected_joyshock_device_ids.erase(device_id);
            }
            ++i;
        }
    }

    bool join_joycons_by_device_id(i32 device_id, i32 device_id_2)
    {
        bool is_valid = false;
        i32 index = -1;
        i32 index_2 = -1;
        i32 i = 0;
        while (i < joyshock_devices.size() && (index == -1 || index_2 == -1))
        {
            if (joyshock_devices[i].device_id == device_id)
            {
                index = i;
            }
            else if (joyshock_devices[i].device_id == device_id_2)
            {
                index_2 = i;
            }

            if (index != -1 && index_2 != -1)
            {
                i32 device_type = joyshock_devices[index].device_type;
                i32 device_type_2 = joyshock_devices[index_2].device_type;
                is_valid = (device_type == JS_TYPE_JOYCON_LEFT && device_type_2 == JS_TYPE_JOYCON_RIGHT) ||
                    (device_type == JS_TYPE_JOYCON_RIGHT && device_type_2 == JS_TYPE_JOYCON_LEFT) && !joyshock_devices[index].is_joycon_pair && !joyshock_devices[index_2].is_joycon_pair;
                if (is_valid)
                {
                    register_input_device(joyshock_devices[index].player_id, device_id_2, device_type_2, index);
                    set_player_number_light(joyshock_devices[index].device_id, joyshock_devices[index].device_type, joyshock_devices[index].player_id + 1, joyshock_devices[index].is_joycon_pair);
                }
            }
            ++i;
        }

        return is_valid;
    }

    bool join_joycons_by_player_id(i32 player_id, i32 player_id_2)
    {
        bool is_valid = false;
        i32 index = -1;
        i32 index_2 = -1;
        i32 i = 0;
        while (i < joyshock_devices.size() && (index == -1 || index_2 == -1))
        {
            if (joyshock_devices[i].player_id == player_id)
            {
                index = i;
            }
            else if (joyshock_devices[i].player_id == player_id_2)
            {
                index_2 = i;
            }

            if (index != -1 && index_2 != -1)
            {
                i32 device_type = joyshock_devices[index].device_type;
                i32 device_type2 = joyshock_devices[index_2].device_type;
                is_valid = (device_type == JS_TYPE_JOYCON_LEFT && device_type2 == JS_TYPE_JOYCON_RIGHT) ||
                    (device_type == JS_TYPE_JOYCON_RIGHT && device_type2 == JS_TYPE_JOYCON_LEFT) && !joyshock_devices[index].is_joycon_pair && !joyshock_devices[index_2].is_joycon_pair;
                if (is_valid)
                {
                    register_input_device(joyshock_devices[index].player_id, joyshock_devices[index_2].device_id, device_type2, index);
                    set_player_number_light(joyshock_devices[index].device_id, joyshock_devices[index].device_type, joyshock_devices[index].player_id + 1, joyshock_devices[index].is_joycon_pair);
                }
            }
            ++i;
        }

        return is_valid;
    }

    bool query_connected_devices()
    {
        i32 old_connected_devices = connected_joyshock_devices;
        connected_joyshock_devices = JslConnectDevices();

        if (connected_joyshock_devices != old_connected_devices)
        {
            if (connected_joyshock_devices == 0)
            {
                joyshock_devices.clear();
                connected_joyshock_device_ids.clear();
                return 0;
            }

            Array<i32> device_ids;
            device_ids.resize(connected_joyshock_devices);
            JslGetConnectedDeviceHandles(device_ids.data(), connected_joyshock_devices);

            for (i32 i = 0; i < device_ids.size(); ++i)
            {
                i32 player_id = *player_ids.begin();
                player_ids.erase(player_id);
                register_input_device(player_id, device_ids[i], JslGetControllerType(device_ids[i]));
                JslSetAutomaticCalibration(device_ids[i], true);
            }
            UnorderedSet<i32> devices_to_remove;
            for (i32 device_id : connected_joyshock_device_ids)
            {
                if (std::find(device_ids.begin(), device_ids.end(), device_id) == device_ids.end())
                {
                    devices_to_remove.insert(device_id);
                }
            }
            for (i32 deviceId : devices_to_remove)
            {
                remove_registered_input_device_by_device_id(deviceId);
            }
            join_joycons_by_device_id(0, 1);

            return true;
        }
        return false;
    }


    void disconnect_all_joyshock_devices()
    {
        JslDisconnectAndDisposeAll();

        joyshock_devices.clear();
        connected_joyshock_device_ids.clear();
        connected_joyshock_devices = 0;
    }

    void get_device_id_and_index_by_player_id(i32 player_id, i32& device_id, i32& device_id_2, i32& index)
    {
        bool found = false;
        i32 i = 0;
        device_id = -1;
        device_id_2 = -1;
        index = -1;

        while (i < joyshock_devices.size() && !found)
        {
            found = joyshock_devices[i].player_id == player_id;

            if (found)
            {
                device_id = joyshock_devices[i].device_id;
                device_id_2 = joyshock_devices[i].device_id_2;
                index = i;
            }
            ++i;
        }
    }

    void get_player_id_and_index_by_device_id(i32 device_id, i32& player_id, i32& index)
    {
        bool found = false;
        i32 i = 0;
        player_id = -1;
        index = -1;

        while (i < joyshock_devices.size() && !found)
        {
            found = joyshock_devices[i].device_id == device_id;

            if (found)
            {
                player_id = joyshock_devices[i].player_id;
                index = i;
            }
            ++i;
        }
    }

    bool set_player_id_by_device_id(i32 device_id, i32 player_id)
    {
        bool found = false;
        i32 i = 0;

        while (i < joyshock_devices.size() && !found)
        {
            found = joyshock_devices[i].device_id == device_id || joyshock_devices[i].device_id_2 == device_id;

            if (found)
            {
                joyshock_devices[i].player_id = player_id;
            }
            ++i;
        }

        return found;
    }

    i32 get_num_joyshock_devices()
    {
        return connected_joyshock_devices;
    }

    size_t get_num_joyshock_players()
    {
        return joyshock_devices.size();
    }

    bool supports_rumble(i32 player_id)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return false;

        return joyshock_devices[index].device_type == JS_TYPE_DS ||
            joyshock_devices[index].device_type == JS_TYPE_DS4 ||
            joyshock_devices[index].device_type == JS_TYPE_DS3;
    }

    void set_rumble(i32 player_id, const RumbleValues& rumble_values)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        if (rumble_values == joyshock_devices[index].last_rumble_values)
        {
            joyshock_devices[index].repeat_rumble_timer += delta_seconds();
            if (joyshock_devices[index].repeat_rumble_timer < REPEAT_RUMBLE_TIME) return;
        }

        joyshock_devices[index].repeat_rumble_timer = 0.f;
        joyshock_devices[index].last_rumble_values = rumble_values;

        JslSetRumble(device_id, static_cast<i32>(rumble_values.left_large * 255), static_cast<i32>(rumble_values.right_large * 255));
    }

    bool supports_hd_rumble(i32 player_id)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return false;

        return joyshock_devices[index].device_type == JS_TYPE_JOYCON_LEFT ||
            joyshock_devices[index].device_type == JS_TYPE_JOYCON_RIGHT ||
            joyshock_devices[index].device_type == JS_TYPE_PRO_CONTROLLER;
    }

    void enable_hd_rumble(i32 player_id)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);

        JslEnableHDRumble(device_id);
        JslEnableHDRumble(device_id_2);
    }

    void disable_hd_rumble(i32 _controllerId)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(_controllerId, device_id, device_id_2, index);
        if (index < 0) return;

        JslDisableHDRumble(device_id);
        JslDisableHDRumble(device_id_2);

    }

    void set_hd_rumble(i32 player_id, const HDRumbleValues& rumble_values)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        if (rumble_values == joyshock_devices[index].last_HD_rumble_values)
        {
            joyshock_devices[index].repeat_rumble_timer += delta_seconds();
            joyshock_devices[index].repeat_rumble_timer_2 += delta_seconds();
            if (joyshock_devices[index].repeat_rumble_timer < REPEAT_RUMBLE_TIME) return;
        }

        joyshock_devices[index].repeat_rumble_timer = 0.f;
        joyshock_devices[index].repeat_rumble_timer_2 = 0.f;
        joyshock_devices[index].last_HD_rumble_values = rumble_values;
        joyshock_devices[index].last_HD_rumble_values_2 = rumble_values;

        JslSetHDRumble(device_id, rumble_values.low_freq, rumble_values.low_ampli, rumble_values.high_freq, rumble_values.high_ampli);
        JslSetHDRumble(device_id_2, rumble_values.low_freq, rumble_values.low_ampli, rumble_values.high_freq, rumble_values.high_ampli);

    }

    void set_hd_rumble_LR(i32 player_id, const HDRumbleValues& rumble_values_L, const HDRumbleValues& rumble_values_R)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        if (rumble_values_L == joyshock_devices[index].last_HD_rumble_values && joyshock_devices[index].repeat_rumble_timer < REPEAT_RUMBLE_TIME)
        {
            joyshock_devices[index].repeat_rumble_timer += delta_seconds();
        }
        else
        {
            joyshock_devices[index].repeat_rumble_timer = 0.f;
            joyshock_devices[index].last_HD_rumble_values = rumble_values_L;

            JslSetHDRumble(device_id, rumble_values_L.low_freq, rumble_values_L.low_ampli, rumble_values_L.high_freq, rumble_values_L.high_ampli);
        }
        if (device_id_2 == -1) return;

        if (rumble_values_R == joyshock_devices[index].last_HD_rumble_values_2 && joyshock_devices[index].repeat_rumble_timer_2 < REPEAT_RUMBLE_TIME)
        {
            joyshock_devices[index].repeat_rumble_timer_2 += delta_seconds();
        }
        else
        {
            joyshock_devices[index].repeat_rumble_timer_2 = 0.f;
            joyshock_devices[index].last_HD_rumble_values_2 = rumble_values_R;

            JslSetHDRumble(device_id_2, rumble_values_R.low_freq, rumble_values_R.low_ampli, rumble_values_R.high_freq, rumble_values_R.high_ampli);
        }
    }

    void set_ds5_triggers_off(i32 player_id, AffectedTriggers affected_triggers)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersOff(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers));
    }

    void set_ds5_triggers_feedback(i32 player_id, AffectedTriggers affected_triggers, u8 position, u8 strength)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersFeedback(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), position, strength);
    }

    void set_ds5_triggers_weapon(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 strength)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersWeapon(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), start_position, end_position, strength);
    }

    void set_ds5_triggers_vibration(i32 player_id, AffectedTriggers affected_triggers, u8 position, u8 amplitude, u8 frequency)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersVibration(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), position, amplitude, frequency);
    }

    void set_ds5_triggers_multi_pos_feedback(i32 player_id, AffectedTriggers affected_triggers, const Array<u8>& strength)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersMultiPosFeedback(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), strength);
    }

    void set_ds5_triggers_slope_feedback(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 start_strength, u8 end_strength)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersSlopeFeedback(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), start_position, end_position, start_strength, end_strength);
    }

    void set_ds5_triggers_multi_pos_vibration(i32 player_id, AffectedTriggers affected_triggers, u8 frequency, const Array<u8>& amplitude)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersMultiPosVibration(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), frequency, amplitude);
    }

    void set_ds5_triggers_bow(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 strength, u8 snap_force)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersBow(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), start_position, end_position, strength, snap_force);
    }

    void set_ds5_triggers_galloping(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 first_foot, u8 second_foot, u8 frequency)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersGalloping(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), start_position, end_position, first_foot, second_foot, frequency);
    }

    void set_ds5_triggers_machine(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 amplitude_A, u8 amplitude_B, u8 frequency, u8 period)
    {
        i32 device_id, device_id_2, index;
        get_device_id_and_index_by_player_id(player_id, device_id, device_id_2, index);
        if (index < 0) return;

        JslSetDS5TriggersMachine(device_id, static_cast<EDS5AffectedTriggers>(affected_triggers), start_position, end_position, amplitude_A, amplitude_B, frequency, period);
    }

    ListenerAction start()
    {
        // -------------------------
        // GLFW INPUT
        // -------------------------

        for(i32 i = 0; i < NIT_MAX_PLAYERS; ++i)
        {
            const char* gamepad_name = glfwGetJoystickName(i);
            if (gamepad_name == nullptr) continue;

            if (gamepad_name[0] == 'X') // jiji 😇
            {
                i32 player_id = *player_ids.begin();
                player_ids.erase(player_id);
                GLFWDeviceInfo device_info = { player_id, i };

                glfw_devices.push_back(device_info);
            }
        }

        // -------------------------
        // JOYSHOCK INPUT
        // -------------------------
        init_joyshock();

        input_registry->input_actions = asset_get_pool<InputAction>();

        return ListenerAction::StayListening;
    }

    ListenerAction update()
    {
        // -------------------------
        // KEYBOARD INPUT
        // -------------------------

        // For each button check against the previous state and send the correct message if any
        for (i32 i = 0; i < g_num_keyboard_keys; ++i)
        {
            bool key_pressed = is_key_pressed(keyboard_key_codes[i]);

            handle_button_press(key_pressed, keyboard_keys[i], 0);
        }

        // ------------------------
        // GLFW INPUT
        // ------------------------
        
        for(i32 i = 0; i < glfw_devices.size(); ++i)
        {
            i32 gamepad_buttons_count = 0;
            i32 gamepad_axis_count = 0;
            const u8* buttons = glfwGetJoystickButtons(glfw_devices[i].device_id, &gamepad_buttons_count);
            const f32* axes = glfwGetJoystickAxes(glfw_devices[i].device_id, &gamepad_axis_count);

            // For each button check against the previous state and send the correct message if any
            for (i32 j = 0; j < gamepad_buttons_count; ++j)
            {
                handle_button_press(buttons[j] == GLFW_PRESS, glfw_button_keys[j], glfw_devices[i].player_id);
            }

            for (i32 j = 0; j < gamepad_axis_count; ++j)
            {
                handle_axis(axes[j], glfw_axis_keys[j], glfw_devices[i].player_id);
            }
        }


        // -------------------------
        // JOYSHOCK INPUT
        // -------------------------

        for (i32 i = 0; i < joyshock_devices.size(); ++i)
        {
            handle_joyshock_controller_events(joyshock_devices[i].player_id, joyshock_devices[i].device_id, joyshock_devices[i].device_type, joyshock_devices[i].is_joycon_pair, joyshock_devices[i].last_button_data);
            if (joyshock_devices[i].is_joycon_pair)
            {
                handle_joyshock_controller_events(joyshock_devices[i].player_id, joyshock_devices[i].device_id_2, joyshock_devices[i].device_type_2, joyshock_devices[i].is_joycon_pair, joyshock_devices[i].last_button_data_2);
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

            if (input_action_pool[i].input_type == InputType::Digital)
            {
                InputContent button_state;
                get_button_state(input_action_pool[i].key, button_state);
                if (!button_state.is_dirty) continue;
                button_state.is_dirty = false;

                if (button_state.current_digital_value != button_state.last_digital_value)
                {
                    if (button_state.current_digital_value)
                    {
                        on_controller_button_pressed(&input_action_pool[i], false);
                    }
                    else
                    {
                        on_controller_button_released(&input_action_pool[i], false);
                    }
                }
                else if (button_state.current_digital_value)
                {
                    on_controller_button_pressed(&input_action_pool[i], true);
                }
            }
            else if (input_action_pool[i].input_type == InputType::Axis1D)
            {
                InputContent button_state;
                get_button_state(input_action_pool[i].key, button_state);
                if (!button_state.is_dirty) continue;
                button_state.is_dirty = false;

                on_controller_analog(&input_action_pool[i], button_state.current_analog_value);
            }
            else if (input_action_pool[i].input_type == InputType::Axis2D)
            {
                InputContent button_state_x, button_state_y;
                get_button_state(input_action_pool[i].paired_input_key_x, button_state_x);
                get_button_state(input_action_pool[i].paired_input_key_y, button_state_y);
                if (!button_state_x.is_dirty && !button_state_y.is_dirty) continue;
                button_state_x.is_dirty = false;
                button_state_y.is_dirty = false;

                on_controller_vector2(&input_action_pool[i], Vector2(button_state_x.current_analog_value, button_state_y.current_analog_value));
            }
            else if (input_action_pool[i].input_type == InputType::Axis3D)
            {
                InputContent button_state_x, button_state_y, button_state_z;
                get_button_state(input_action_pool[i].paired_input_key_x, button_state_x);
                get_button_state(input_action_pool[i].paired_input_key_y, button_state_y);
                get_button_state(input_action_pool[i].paired_input_key_z, button_state_z);
                if (!button_state_x.is_dirty && !button_state_y.is_dirty && !button_state_z.is_dirty) continue;
                button_state_x.is_dirty = false;
                button_state_y.is_dirty = false;
                button_state_z.is_dirty = false;

                on_controller_vector3(&input_action_pool[i], Vector3(button_state_x.current_analog_value, button_state_y.current_analog_value, button_state_z.current_analog_value));
            }
            else if (input_action_pool[i].input_type == InputType::Axis4D)
            {
                InputContent button_state_x, button_state_y, button_state_z, button_state_w;
                get_button_state(input_action_pool[i].paired_input_key_x, button_state_x);
                get_button_state(input_action_pool[i].paired_input_key_y, button_state_y);
                get_button_state(input_action_pool[i].paired_input_key_z, button_state_z);
                get_button_state(input_action_pool[i].paired_input_key_w, button_state_w);
                if (!button_state_x.is_dirty && !button_state_y.is_dirty && !button_state_z.is_dirty && !button_state_w.is_dirty) continue;
                button_state_x.is_dirty = false;
                button_state_y.is_dirty = false;
                button_state_z.is_dirty = false;
                button_state_w.is_dirty = false;

                on_controller_vector4(&input_action_pool[i], Vector4(button_state_x.current_analog_value,
                    button_state_y.current_analog_value,
                    button_state_z.current_analog_value,
                    button_state_w.current_analog_value));
            }
        }

        // -------------------------
        // INPUT MODIFIERS
        // -------------------------

        for (u32 i = 0; i < input_registry->next_input_modifier_type_index - 1; ++i)
        {
            InputModifierPool& input_modifier_data = input_registry->input_modifier_pool[i];
            if (input_modifier_data.type_index == 0 || input_modifier_data.type_index > NIT_MAX_INPUT_MODIFIER_TYPES) continue;
            for (u32 j = 0; j < input_modifier_data.data_pool.sparse_set.count; ++j)
            {
                GamepadKeys action_key = input_modifier_data.modifier_info[j].action_key;
                void* data = pool_get_raw_data(&input_modifier_data.data_pool, j);

                if (!input_action_context_map.contains(action_key)) continue;

                InputActionContext context = input_action_context_map[action_key];

                input_modifier_data.fn_invoke_modify(data, context.input_value, context.input_type);

                input_action_context_map[action_key] = context;
            }
        }

        // -------------------------
        // INPUT ACTIONS
        // -------------------------
        input_action_pool = (InputAction*)(input_registry->input_actions->data_pool.elements);
        for (u32 i = 0; i < input_registry->input_actions->data_pool.sparse_set.count; ++i)
        {
            if (!&input_action_pool[i] || input_action_pool[i].input_type == InputType::Digital) continue;

            InputActionContext context;
            get_input_action_context(input_action_pool[i].key, context);

            event_broadcast<const InputActionContext&>(input_action_pool[i].input_performed_event, context);
        }

        return ListenerAction::StayListening;
    }

    void on_controller_analog(InputAction* input_action, f32 analog_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Axis1D;
        input_context.input_value = Vector4(analog_value, 0.f, 0.f, 0.f);

        input_context.player_id = 1;
    }

    void on_controller_vector2(InputAction* input_action, const Vector2& vector2_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Axis2D;
        input_context.input_value = Vector4(vector2_value.x, vector2_value.y, 0.f, 0.f);

        input_context.player_id = 1;
    }

    void on_controller_vector3(InputAction* input_action, const Vector3& vector3_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Axis3D;
        input_context.input_value = Vector4(vector3_value.x, vector3_value.y, vector3_value.z, 0.f);

        input_context.player_id = 1;
    }

    void on_controller_vector4(InputAction* input_action, const Vector4& vector4_value)
    {
        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Axis4D;
        input_context.input_value = Vector4(vector4_value.x, vector4_value.y, vector4_value.z, vector4_value.w);

        input_context.player_id = 1;
    }

    void on_controller_button_pressed(InputAction* input_action, bool is_repeat)
    {
        if ((input_action->trigger_type == TriggerType::Pressed && is_repeat) || input_action->trigger_type == TriggerType::Released) return;

        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Digital;
        input_context.input_value = Vector4(1.f, 0.f, 0.f, 0.f);

        input_context.is_pressed = true;
        input_context.is_repeat = is_repeat;
        input_context.player_id = 1;

        event_broadcast<const InputActionContext&>(input_action->input_performed_event, input_context);
    }

    void on_controller_button_released(InputAction* input_action, bool is_repeat)
    {
        if (input_action->trigger_type == TriggerType::Pressed || input_action->trigger_type == TriggerType::Down) return;

        InputActionContext& input_context = input_action_context_map[input_action->key];
        input_context.input_type = InputType::Digital;
        input_context.input_value = Vector4(0.f, 0.f, 0.f, 0.f);

        input_context.is_pressed = false;
        input_context.is_repeat = is_repeat;
        input_context.player_id = 1;

        event_broadcast<const InputActionContext&>(input_action->input_performed_event, input_context);
    }

    void handle_button_press(bool pressed, GamepadKeys key, i32 player_id)
    {
        InputContent& input_content = controller_state[key];
        input_content.last_digital_value = input_content.current_digital_value;
        input_content.current_digital_value = pressed;
        input_content.current_analog_value = input_content.current_digital_value;
        input_content.is_dirty = true;
    }

    void handle_axis(f32 value, GamepadKeys key, i32 player_id)
    {
        InputContent& input_content = controller_state[key];
        input_content.last_digital_value = input_content.current_digital_value;
        input_content.current_digital_value = value > g_LowerDeadzone;
        input_content.current_analog_value = value;
        input_content.is_dirty = true;
    }

    void handle_joyshock_controller_events(i32 player_id, i32 device_id, i32 device_type, bool is_joycon_pair, i32& last_button_data)
    {
        JOY_SHOCK_STATE simple_state = JslGetSimpleState(device_id);

        i32 button_mask = 0x00001;
        if (is_joycon_pair || (device_type != JS_TYPE_JOYCON_LEFT && device_type != JS_TYPE_JOYCON_RIGHT))
        {
            for (i32 i = 0; i < NUM_BUTTONS; ++i)
            {
                handle_button_press(button_mask & simple_state.buttons, button_map[button_mask], player_id);
                button_mask = button_mask << 1;
            }
        }
        switch (device_type)
        {
        case JS_TYPE_JOYCON_LEFT:
            if (!is_joycon_pair)
            {
                handle_button_press(0x00008 & simple_state.buttons, GamepadKeys::GamepadButtonNorth, player_id);
                handle_button_press(0x00004 & simple_state.buttons, GamepadKeys::GamepadButtonSouth, player_id);
                handle_button_press(0x00001 & simple_state.buttons, GamepadKeys::GamepadButtonWest, player_id);
                handle_button_press(0x00002 & simple_state.buttons, GamepadKeys::GamepadButtonEast, player_id);
                handle_button_press(0x00040 & simple_state.buttons, GamepadKeys::GamepadButtonLeftThumb, player_id);
                handle_button_press(0x00100 & simple_state.buttons, GamepadKeys::GamepadButtonLeftBumper, player_id);
                handle_button_press(0x00400 & simple_state.buttons, GamepadKeys::GamepadAxisLeftTrigger, player_id);
                handle_button_press(0x00020 & simple_state.buttons, GamepadKeys::GamepadButtonStart, player_id);
                handle_button_press(0x400000 & simple_state.buttons, GamepadKeys::SwitchButtonSL, player_id);
                handle_button_press(0x800000 & simple_state.buttons, GamepadKeys::SwitchButtonSR, player_id);
            }
            else
            {
                handle_button_press(0x400000 & simple_state.buttons, GamepadKeys::SwitchButtonLeftSL, player_id);
                handle_button_press(0x800000 & simple_state.buttons, GamepadKeys::SwitchButtonLeftSR, player_id);
            }

            handle_button_press(0x200000 & simple_state.buttons, GamepadKeys::SwitchButtonCapture, player_id);
            break;
        case JS_TYPE_JOYCON_RIGHT:
            if (!is_joycon_pair)
            {
                handle_button_press(0x04000 & simple_state.buttons, GamepadKeys::GamepadButtonNorth, player_id);
                handle_button_press(0x02000 & simple_state.buttons, GamepadKeys::GamepadButtonSouth, player_id);
                handle_button_press(0x01000 & simple_state.buttons, GamepadKeys::GamepadButtonWest, player_id);
                handle_button_press(0x08000 & simple_state.buttons, GamepadKeys::GamepadButtonEast, player_id);
                handle_button_press(0x00080 & simple_state.buttons, GamepadKeys::GamepadButtonLeftThumb, player_id);
                handle_button_press(0x00200 & simple_state.buttons, GamepadKeys::GamepadButtonLeftBumper, player_id);
                handle_button_press(0x00800 & simple_state.buttons, GamepadKeys::GamepadAxisLeftTrigger, player_id);
                handle_button_press(0x00010 & simple_state.buttons, GamepadKeys::GamepadButtonStart, player_id);
                handle_button_press(0x400000 & simple_state.buttons, GamepadKeys::SwitchButtonSL, player_id);
                handle_button_press(0x800000 & simple_state.buttons, GamepadKeys::SwitchButtonSR, player_id);
            }
            else
            {
                handle_button_press(0x400000 & simple_state.buttons, GamepadKeys::SwitchButtonRightSL, player_id);
                handle_button_press(0x800000 & simple_state.buttons, GamepadKeys::SwitchButtonRightSR, player_id);
            }

            break;
        case JS_TYPE_PRO_CONTROLLER:
            handle_button_press(0x200000 & simple_state.buttons, GamepadKeys::SwitchButtonCapture, player_id);
            break;
        case JS_TYPE_DS4:
            handle_button_press(0x200000 & simple_state.buttons, GamepadKeys::DualShockButtonTouchpadDown, player_id);
            break;
        case JS_TYPE_DS:
            handle_button_press(0x200000 & simple_state.buttons, GamepadKeys::DualShockButtonCreate, player_id);
            handle_button_press(0x400000 & simple_state.buttons, GamepadKeys::DualShockButtonMic, player_id);
            break;
        }
        if (!is_joycon_pair && (device_type == JS_TYPE_JOYCON_LEFT || device_type == JS_TYPE_JOYCON_RIGHT))
        {
            if (device_type == JS_TYPE_JOYCON_LEFT)
            {
                const f32 aux_stick_x = simple_state.stickLX;
                simple_state.stickLX = -simple_state.stickLY;
                simple_state.stickLY = aux_stick_x;
            }
            if (device_type == JS_TYPE_JOYCON_RIGHT)
            {
                simple_state.stickLX = simple_state.stickRY;
                simple_state.stickLY = -simple_state.stickRX;

                simple_state.stickRX = 0.f;
                simple_state.stickRY = 0.f;
            }
        }
        if (device_type != JS_TYPE_JOYCON_RIGHT || !is_joycon_pair)
        {
            handle_axis(simple_state.stickLX, GamepadKeys::GamepadAxisLeftX, player_id);
            handle_axis(simple_state.stickLY, GamepadKeys::GamepadAxisLeftY, player_id);
            handle_axis(simple_state.lTrigger, GamepadKeys::GamepadAxisLeftTrigger, player_id);
        }
        if (device_type != JS_TYPE_JOYCON_LEFT)
        {
            handle_axis(simple_state.stickRX, GamepadKeys::GamepadAxisRightX, player_id);
            handle_axis(simple_state.stickRY, GamepadKeys::GamepadAxisRightY, player_id);
            handle_axis(simple_state.rTrigger, GamepadKeys::GamepadAxisRightTrigger, player_id);
        }
        if (device_type == JS_TYPE_DS || device_type == JS_TYPE_DS4)
        {
            TOUCH_STATE touch_state = JslGetTouchState(device_id);

            handle_button_press(touch_state.t0Down, GamepadKeys::DualShockButtonFirstTouchpad, player_id);
            handle_button_press(touch_state.t1Down, GamepadKeys::DualShockButtonSecondTouchpad, player_id);
            if (touch_state.t0Down)
            {
                handle_axis(touch_state.t0X, GamepadKeys::DualShockAxisFirstTouchpadX, player_id);
                handle_axis(touch_state.t0Y, GamepadKeys::DualShockAxisFirstTouchpadY, player_id);
            }
            if (touch_state.t1Down)
            {
                handle_axis(touch_state.t1X, GamepadKeys::DualShockAxisSecondTouchpadX, player_id);
                handle_axis(touch_state.t1Y, GamepadKeys::DualShockAxisSecondTouchpadY, player_id);
            }
        }
        if (device_type == JS_TYPE_DS3)
        {
            DS3_STATE ds3_state = JslGetDS3State(device_id);

            handle_axis(ds3_state.dpadUp, GamepadKeys::DualShockAxisDpadUp, player_id);
            handle_axis(ds3_state.dpadRight, GamepadKeys::DualShockAxisDpadRight, player_id);
            handle_axis(ds3_state.dpadDown, GamepadKeys::DualShockAxisDpadDown, player_id);
            handle_axis(ds3_state.dpadLeft, GamepadKeys::DualShockAxisDpadLeft, player_id);
            handle_axis(ds3_state.cross, GamepadKeys::DualShockAxisSouth, player_id);
            handle_axis(ds3_state.circle, GamepadKeys::DualShockAxisEast, player_id);
            handle_axis(ds3_state.square, GamepadKeys::DualShockAxisWest, player_id);
            handle_axis(ds3_state.triangle, GamepadKeys::DualShockAxisNorth, player_id);
            handle_axis(ds3_state.l1, GamepadKeys::DualShockAxisLeftBumper, player_id);
            handle_axis(ds3_state.r1, GamepadKeys::DualShockAxisRightBumper, player_id);
        }

        IMU_STATE imu_state = JslGetIMUState(device_id);
        MOTION_STATE motion_state = JslGetMotionState(device_id);

        if (is_joycon_pair && device_type == JS_TYPE_JOYCON_LEFT)
        {
            handle_axis(imu_state.gyroX, GamepadKeys::SwitchAxisLeftGyroX, player_id);
            handle_axis(imu_state.gyroY, GamepadKeys::SwitchAxisLeftGyroY, player_id);
            handle_axis(imu_state.gyroZ, GamepadKeys::SwitchAxisLeftGyroZ, player_id);

            handle_axis(motion_state.accelX, GamepadKeys::SwitchAxisLeftAccelX, player_id);
            handle_axis(motion_state.accelY, GamepadKeys::SwitchAxisLeftAccelY, player_id);
            handle_axis(motion_state.accelZ, GamepadKeys::SwitchAxisLeftAccelZ, player_id);

            handle_axis(motion_state.gravX, GamepadKeys::SwitchAxisLeftGravX, player_id);
            handle_axis(motion_state.gravY, GamepadKeys::SwitchAxisLeftGravY, player_id);
            handle_axis(motion_state.gravZ, GamepadKeys::SwitchAxisLeftGravZ, player_id);

            handle_axis(motion_state.quatX, GamepadKeys::SwitchAxisLeftQuatX, player_id);
            handle_axis(motion_state.quatY, GamepadKeys::SwitchAxisLeftQuatY, player_id);
            handle_axis(motion_state.quatZ, GamepadKeys::SwitchAxisLeftQuatZ, player_id);
            handle_axis(motion_state.quatW, GamepadKeys::SwitchAxisLeftQuatW, player_id);
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

            handle_axis(imu_state.gyroX, GamepadKeys::GamepadAxisGyroX, player_id);
            handle_axis(imu_state.gyroY, GamepadKeys::GamepadAxisGyroY, player_id);
            handle_axis(imu_state.gyroZ, GamepadKeys::GamepadAxisGyroZ, player_id);
            handle_axis(imu_state.accelX, GamepadKeys::GamepadAxisTotalAccelX, player_id);
            handle_axis(imu_state.accelY, GamepadKeys::GamepadAxisTotalAccelY, player_id);
            handle_axis(imu_state.accelZ, GamepadKeys::GamepadAxisTotalAccelZ, player_id);
            handle_axis(motion_state.accelX, GamepadKeys::GamepadAxisAccelX, player_id);
            handle_axis(motion_state.accelY, GamepadKeys::GamepadAxisAccelY, player_id);
            handle_axis(motion_state.accelZ, GamepadKeys::GamepadAxisAccelZ, player_id);
            handle_axis(motion_state.gravX, GamepadKeys::GamepadAxisGravX, player_id);
            handle_axis(motion_state.gravY, GamepadKeys::GamepadAxisGravY, player_id);
            handle_axis(motion_state.gravZ, GamepadKeys::GamepadAxisGravZ, player_id);
            handle_axis(motion_state.quatX, GamepadKeys::GamepadAxisQuatX, player_id);
            handle_axis(motion_state.quatY, GamepadKeys::GamepadAxisQuatY, player_id);
            handle_axis(motion_state.quatZ, GamepadKeys::GamepadAxisQuatZ, player_id);
            handle_axis(motion_state.quatW, GamepadKeys::GamepadAxisQuatW, player_id);
        }

        last_button_data = simple_state.buttons;
    }
}