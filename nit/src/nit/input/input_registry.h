#pragma once
#include "input_enums.h"
#include "input_keycodes.h"
#include "input_action.h"

#ifndef NIT_MAX_INPUT_ACTIONS
#define NIT_MAX_INPUT_ACTIONS 100
#endif

#ifndef NIT_MAX_PLAYERS
#define NIT_MAX_PLAYERS 10
#endif

namespace nit
{
    struct AssetPool; 

    template<typename T>
    struct InputModifierRegisterArgs
    {
        void (*fn_serialize) (const T*, YAML::Emitter&);
        void (*fn_deserialize) (T*, const YAML::Node&);
        void (*fn_modify) (const T*, Vector4&, InputType);
        NIT_IF_EDITOR_ENABLED(void (*fn_draw_editor) (T*));
    };

    struct InputModifierPool
    {
        u32                                       type_index = 0;
        Pool                                      data_pool;
        InputModifierInfo                         modifier_info[NIT_MAX_INPUT_ACTIONS];

        FnInvokeModify                            fn_invoke_modify;
        Delegate<void(InputAction*, void*, bool)> fn_add_to_input_action;
        Delegate<void(InputAction*)>              fn_remove_from_input_action;
        Delegate<bool(const InputAction*)>        fn_is_in_input_action;
        Delegate<void* (const InputAction*)>      fn_get_from_input_action;
    };

    struct InputModifierAddedArgs
    {
        InputAction* input_action = nullptr;
        Type* type = nullptr;
    };

    struct InputModifierRemovedArgs
    {
        InputAction* input_action = nullptr;
        Type* type = nullptr;
    };

    using InputModifierAddedListener = Listener<const InputModifierAddedArgs&>;
    using InputModifierRemovedListener = Listener<const InputModifierRemovedArgs&>;
    using InputModifierAddedEvent = Event<const InputModifierAddedArgs&>;
    using InputModifierRemovedEvent = Event<const InputModifierRemovedArgs&>;

    struct InputRegistry
    {
        Queue<u32>                   available_input_actions;
        AssetPool*                   input_actions = nullptr;
        InputModifierPool*           input_modifier_pool = nullptr;
        u32                          next_input_modifier_type_index = 1;
        InputModifierAddedEvent      input_modifier_added_event;
        InputModifierRemovedEvent    input_modifier_removed_event;
        u32                          max_input_actions = NIT_MAX_INPUT_ACTIONS;
    };

    void input_registry_set_instance(InputRegistry* input_registry_instance);
    InputRegistry* input_registry_get_instance();

    template<typename T>
    T* add_input_modifier(InputAction* input_action, const T& data = {}, bool invoke_add_event = true);
    template<typename T>
    void remove_input_modifier(InputAction* input_action);

    template<typename T>
    T* get_input_modifier(const InputAction* input_action);

    template<typename T>
    bool has_input_modifier(const InputAction* input_action);


    template<typename T>
    void input_modifier_register(const InputModifierRegisterArgs<T>& args)
    {
        InputRegistry* input_registry = input_registry_get_instance();
        NIT_CHECK_MSG(input_registry->next_input_modifier_type_index <= NIT_MAX_INPUT_MODIFIER_TYPES, "Input modifiers out of range!");

        if (!type_exists<T>())
        {
            type_register<T>({
                .fn_serialize = args.fn_serialize,
                .fn_deserialize = args.fn_deserialize,
                NIT_IF_EDITOR_ENABLED(.fn_draw_editor = args.fn_draw_editor)
                
            });
        }

        InputModifierPool& input_modifier_pool = input_registry->input_modifier_pool[input_registry->next_input_modifier_type_index - 1];
        input_modifier_pool.type_index = input_registry->next_input_modifier_type_index;

        input_modifier_pool.fn_invoke_modify = [fn_modify = args.fn_modify](const void* input_modifier, Vector4& input_value, InputType input_type)
        {
            const T* casted_data = static_cast<const T*>(input_modifier);
            fn_modify(casted_data, input_value, input_type);

        };

        void (*fn_add_to_input_action)(InputAction*, void*, bool) = [](InputAction* input_action, void* data, bool invoke_add_event)
        {
            if (!data)
            {
                add_input_modifier<T>(input_action, T{}, invoke_add_event);
                return;
            }

            T* casted_data = static_cast<T*>(data);
            add_input_modifier<T>(input_action, *casted_data, invoke_add_event);
        };

        void (*fn_remove_from_input_action)(InputAction*) = [](InputAction* input_action)
        {
            remove_input_modifier<T>(input_action);
        };

        bool (*fn_is_in_input_action)(const InputAction*) = [](const InputAction* input_action)
        {
            return has_input_modifier<T>(input_action);
        };

        void* (*fn_get_from_input_action)(const InputAction*) = [](const InputAction* input_action) -> void*
        {
            return get_input_modifier<T>(input_action);
        };

        delegate_bind(input_modifier_pool.fn_add_to_input_action, fn_add_to_input_action);
        delegate_bind(input_modifier_pool.fn_remove_from_input_action, fn_remove_from_input_action);
        delegate_bind(input_modifier_pool.fn_is_in_input_action, fn_is_in_input_action);
        delegate_bind(input_modifier_pool.fn_get_from_input_action, fn_get_from_input_action);

        pool_load<T>(&input_modifier_pool.data_pool, input_registry->max_input_actions);

        ++input_registry->next_input_modifier_type_index;
    }

    template<typename T>
    u32 input_modifier_type_index()
    {
        InputModifierPool* input_modifier_pool = input_find_modifier_pool<T>();
        NIT_CHECK_MSG(input_modifier_pool, "Input Modifier type is not registered!");
        return input_modifier_pool->type_index;
    }


    void input_registry_init();
    void input_registry_finish();
    InputModifierPool* input_find_modifier_pool(const Type* type);

    template<typename T>
    InputModifierPool* input_find_modifier_pool()
    {
        return input_find_modifier_pool(type_get<T>());
    }

    template <typename T>
    T* add_input_modifier(InputAction* input_action, const T& data, bool invoke_add_event)
    {
        NIT_CHECK_MSG(input_action, "Invalid input action!");
        InputModifierPool* input_modifier_pool = input_find_modifier_pool<T>();
        NIT_CHECK_MSG(input_modifier_pool, "Invalid input modifier type!");

        u32 modifier_id;
        T* element = pool_insert_data(&input_modifier_pool->data_pool, modifier_id, data);

        input_action->input_modifiers[input_modifier_pool->type_index] = modifier_id;
        input_action->signature.set(input_modifier_type_index<T>(), true);

        InputModifierInfo& input_modifier_info = input_modifier_pool->modifier_info[modifier_id];
        input_modifier_info.action_key = input_action->key;

        InputModifierAddedArgs args;
        args.input_action = input_action;
        args.type = input_modifier_pool->data_pool.type;

        if (invoke_add_event)
        {
            event_broadcast<const InputModifierAddedArgs&>(input_registry_get_instance()->input_modifier_added_event, args);
        }
        return element;
    }

    template <typename T>
    void remove_input_modifier(InputAction* input_action)
    {
        NIT_CHECK_MSG(input_action, "Invalid input action!");
        InputModifierPool* input_modifier_pool = input_find_modifier_pool<T>();
        NIT_CHECK_MSG(input_modifier_pool, "Invalid input modifier type!");

        InputModifierRemovedArgs args;
        args.input_action = input_action;
        args.type = input_modifier_pool->data_pool.type;
        event_broadcast<const InputModifierRemovedArgs&>(input_registry_get_instance()->input_modifier_removed_event, args);

        u32 modifier_id = input_action->input_modifiers[input_modifier_pool->type_index];
        pool_delete_data(&input_modifier_pool->data_pool, modifier_id);
        input_action->input_modifiers[input_modifier_pool->type_index] = 0;
        input_action->signature.set(input_modifier_type_index<T>(), false);
    }

    template <typename T>
    T* get_input_modifier(const InputAction* input_action)
    {
        NIT_CHECK_MSG(input_action, "Invalid input action!");
        InputModifierPool* input_modifier_pool = input_find_modifier_pool<T>();
        NIT_CHECK_MSG(input_modifier_pool, "Invalid input modifier type!");
        u32 modifier_id = input_action->input_modifiers[input_modifier_pool->type_index];
        return pool_get_data<T>(&input_modifier_pool->data_pool, modifier_id);
    }

    template <typename T>
    bool has_input_modifier(const InputAction* input_action)
    {
        NIT_CHECK_MSG(input_action, "Invalid input action!");
        return input_action->signature.test(input_modifier_type_index<T>());
    }

    // ------------------------
    // INPUT SYSTEM
    // ------------------------

    //void SendForceFeedback(i32 _controllerId, f32 _leftMotor, f32 _rightMotor);

    // ------------------------
    // INPUT
    // ------------------------

    bool is_key_pressed(u32 key);
    void get_button_state(GamepadKeys key, InputContent& input_content);
    void get_input_action_context(GamepadKeys key, InputActionContext& input_action_context);
    void set_input_enabled(bool consumed);
    bool is_input_enabled();
    //bool IsMouseButtonPressed(i32 button);

    // ------------------------
    // GLFW
    // ------------------------


    struct GLFWDeviceInfo
    {
        i32 player_id = -1;
        i32 device_id = -1;
    };

    // ------------------------
    // JOYSHOCK
    // ------------------------

    struct JoyShockDeviceInfo
    {
        bool is_joycon_pair = false;
        i32 player_id = -1;
        i32 device_id = -1;
        i32 device_type = -1;
        i32 device_id_2 = -1;
        i32 device_type_2 = -1;
    };

    struct ControllerColor
    {
        Vector3 body_color = V3_ZERO;
        Vector3 left_button_color = V3_ZERO;
        Vector3 right_button_color = V3_ZERO;
        Vector3 left_grip_color = V3_ZERO;
        Vector3 right_grip_color = V3_ZERO;
    };

    struct RumbleValues
    {
        f32 left_large = 0.f;
        f32 left_small = 0.f;
        f32 right_large = 0.f;
        f32 right_small = 0.f;

        inline bool operator==(const RumbleValues& other)
        {
            return left_large == other.left_large && left_small == other.left_small &&
                right_large == other.right_large && right_small == other.right_small;
        }
    };

    struct HDRumbleValues
    {
        f32 low_freq = 0.f;
        f32 low_ampli = 0.f;
        f32 high_freq = 0.f;
        f32 high_ampli = 0.f;

        inline bool operator==(const HDRumbleValues& other)
        {
            return low_freq == other.low_freq && low_ampli == other.low_ampli &&
                high_freq == other.high_freq && high_ampli == other.high_ampli;
        }
    };

    struct JoyShockDeviceEntry
    {
        bool is_joycon_pair = false;

        i32 player_id = -1;

        i32 device_id = -1;
        i32 device_type = -1;
        i32 last_button_data = 0x000000;

        i32 device_id_2 = -1;
        i32 device_type_2 = -1;
        i32 last_button_data_2 = 0x000000;

        RumbleValues last_rumble_values;
        HDRumbleValues last_HD_rumble_values;
        HDRumbleValues last_HD_rumble_values_2;

        float repeat_rumble_timer = 0.f;
        float repeat_rumble_timer_2 = 0.f;

        inline bool operator==(const JoyShockDeviceEntry& other) { return device_id == other.device_id; }
    };


    ControllerColor get_controller_colors_by_player_id(i32 player_id);
    ControllerColor get_controller_colors_by_device_id(i32 device_id);
    void set_controller_light_color_by_player_id(i32 player_id, const Vector3& color);
    void set_controller_light_color_by_device_id(i32 device_id, const Vector3& color);

    bool join_joycons_by_device_id(i32 device_id, i32 device_id_2);
    bool join_joycons_by_player_id(i32 player_id, i32 player_id_2);
    bool query_connected_devices();
    void disconnect_all_joyshock_devices();
    void get_device_id_and_index_by_player_id(i32 player_id, i32& device_id, i32& device_id_2, i32& index);
    void get_player_id_and_index_by_device_id(i32 device_id, i32& player_id, i32& index);
    bool set_player_id_by_device_id(i32 device_id, i32 player_id);

    i32 get_num_joyshock_devices();
    size_t get_num_joyshock_players();
    bool supports_rumble(i32 player_id);
    void set_rumble(i32 player_id, const RumbleValues& rumble_values);
    bool supports_hd_rumble(i32 player_id);
    void enable_hd_rumble(i32 player_id);
    void disable_hd_rumble(i32 player_id);
    void set_hd_rumble(i32 player_id, const HDRumbleValues& rumble_values);
    void set_hd_rumble_LR(i32 player_id, const HDRumbleValues& rumble_values_L, const HDRumbleValues& rumble_values_R);

    // Official effects
    void set_ds5_triggers_off(i32 player_id, AffectedTriggers affected_triggers);
    void set_ds5_triggers_feedback(i32 player_id, AffectedTriggers affected_triggers, u8 position, u8 strength);
    void set_ds5_triggers_weapon(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 strength);
    void set_ds5_triggers_vibration(i32 player_id, AffectedTriggers affected_triggers, u8 position, u8 amplitude, u8 frequency);
    void set_ds5_triggers_multi_pos_feedback(i32 player_id, AffectedTriggers affected_triggers, const Array<u8>& strength);
    void set_ds5_triggers_slope_feedback(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 start_strength, u8 end_strength);
    void set_ds5_triggers_multi_pos_vibration(i32 player_id, AffectedTriggers affected_triggers, u8 frequency, const Array<u8>& amplitude);
    // Not official effects
    void set_ds5_triggers_bow(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 strength, u8 snap_force);
    void set_ds5_triggers_galloping(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 first_foot, u8 second_foot, u8 frequency);
    void set_ds5_triggers_machine(i32 player_id, AffectedTriggers affected_triggers, u8 start_position, u8 end_position, u8 amplitude_A, u8 amplitude_B, u8 frequency, u8 period);


    //JoyShockDeviceConnected& OnJoyShockDeviceConnected();
    //JoyShockDeviceDisconnected& OnJoyShockDeviceDisconnected();
}
