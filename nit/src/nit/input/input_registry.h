#pragma once
#include "input_enums.h"
#include "input_keycodes.h"
#include "input_action.h"

#ifndef NIT_MAX_INPUT_ACTIONS
#define NIT_MAX_INPUT_ACTIONS 100
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
        input_modifier_info.action_key = input_action->m_Key;

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

    bool IsKeyPressed(u32 key);
    void GetButtonState(GamepadKeys key, InputContent& input_content);
    void get_input_action_context(GamepadKeys key, InputActionContext& input_action_context);
    void SetEnabled(bool consumed);
    bool IsEnabled();
    //bool IsMouseButtonPressed(i32 button);

    // ------------------------
    // JOYSHOCK
    // ------------------------

    struct JoyShockDeviceInfo
    {
        bool bIsJoyconPair = false;
        i32 playerId = -1;
        i32 deviceId = -1;
        i32 deviceType = -1;
        i32 deviceId2 = -1;
        i32 deviceType2 = -1;
    };

    struct ControllerColor
    {
        Vector3 bodyColor = V3_ZERO;
        Vector3 leftButtonColor = V3_ZERO;
        Vector3 rightButtonColor = V3_ZERO;
        Vector3 leftGripColor = V3_ZERO;
        Vector3 rightGripColor = V3_ZERO;
    };

    struct RumbleValues
    {
        f32 leftLarge = 0.f;
        f32 leftSmall = 0.f;
        f32 rightLarge = 0.f;
        f32 rightSmall = 0.f;

        inline bool operator==(const RumbleValues& _other)
        {
            return leftLarge == _other.leftLarge && leftSmall == _other.leftSmall &&
                rightLarge == _other.rightLarge && rightSmall == _other.rightSmall;
        }
    };

    struct HDRumbleValues
    {
        f32 lowFreq = 0.f;
        f32 lowAmpli = 0.f;
        f32 highFreq = 0.f;
        f32 highAmpli = 0.f;

        inline bool operator==(const HDRumbleValues& _other)
        {
            return lowFreq == _other.lowFreq && lowAmpli == _other.lowAmpli &&
                highFreq == _other.highFreq && highAmpli == _other.highAmpli;
        }
    };

    struct JoyShockDeviceEntry
    {
        bool bIsJoyconPair = false;

        i32 playerId = -1;

        i32 deviceId = -1;
        i32 deviceType = -1;
        i32 lastButtonData = 0x000000;

        i32 deviceId2 = -1;
        i32 deviceType2 = -1;
        i32 lastButtonData2 = 0x000000;

        RumbleValues lastRumbleValues;
        HDRumbleValues lastHDRumbleValues;
        HDRumbleValues lastHDRumbleValues2;

        float repeatRumbleTimer = 0.f;
        float repeatRumbleTimer2 = 0.f;

        inline bool operator==(const JoyShockDeviceEntry& _other) { return deviceId == _other.deviceId; }
    };

    /*
    Array<JoyShockDeviceInfo> GetRegisteredDevices();
    ControllerColor GetControllerColorsByControllerId(i32 _controllerId);
    ControllerColor GetControllerColorsByDeviceId(i32 _deviceId);
    void SetControllerLightColorByControllerId(i32 _controllerId, const Vector3& _color);
    void SetControllerLightColorByDeviceId(i32 _deviceId, const Vector3& _color);
    */
    bool JoinJoyconsByDeviceId(i32 device_id, i32 device_id_2);
    /*
    bool JoinJoyconsByControllerId(i32 _controllerId, i32 _controllerId2);
    */
    bool QueryConnectedDevices();
    /*
    void DisconnectAllDevices();
    void GetDeviceIdAndIndexByPlayerId(i32 _playerId, i32& deviceId_, i32& deviceId2_, i32& index_);
    */
    void GetPlayerIdAndIndexByDeviceId(i32 _deviceId, i32& playerId_, i32& index_);
    bool SetPlayerIdByDeviceId(i32 device_id, i32 player_id);
    /*
    i32 GetNumJoyShockDevices();
    size_t GetNumJoyShockPlayers();
    bool SupportsRumble(i32 _controllerId);
    void SetRumble(i32 _controllerId, const RumbleValues& _rumbleValues);
    bool SupportsHDRumble(i32 _controllerId);
    void EnableHDRumble(i32 _controllerId);
    void DisableHDRumble(i32 _controllerId);
    void SetHDRumble(i32 _controllerId, const HDRumbleValues& _rumbleValues);
    void SetHDRumbleLR(i32 _controllerId, const HDRumbleValues& _rumbleValuesL, const HDRumbleValues& _rumbleValuesR);

    // Official effects
    void SetDS5TriggersOff(i32 _controllerId, AffectedTriggers affectedTriggers);
    void SetDS5TriggersFeedback(i32 _controllerId, AffectedTriggers affectedTriggers, u8 position, u8 strength);
    void SetDS5TriggersWeapon(i32 _controllerId, AffectedTriggers affectedTriggers, u8 startPosition, u8 endPosition, u8 strength);
    void SetDS5TriggersVibration(i32 _controllerId, AffectedTriggers affectedTriggers, u8 position, u8 amplitude, u8 frequency);
    void SetDS5TriggersMultiPosFeedback(i32 _controllerId, AffectedTriggers affectedTriggers, const Array<u8>& strength);
    void SetDS5TriggersSlopeFeedback(i32 _controllerId, AffectedTriggers affectedTriggers, u8 startPosition, u8 endPosition, u8 startStrength, u8 endStrength);
    void SetDS5TriggersMultiPosVibration(i32 _controllerId, AffectedTriggers affectedTriggers, u8 frequency, const Array<u8>& amplitude);
    // Not official effects
    void SetDS5TriggersBow(i32 _controllerId, AffectedTriggers affectedTriggers, u8 startPosition, u8 endPosition, u8 strength, u8 snapForce);
    void SetDS5TriggersGalloping(i32 _controllerId, AffectedTriggers affectedTriggers, u8 startPosition, u8 endPosition, u8 firstFoot, u8 secondFoot, u8 frequency);
    void SetDS5TriggersMachine(i32 _controllerId, AffectedTriggers affectedTriggers, u8 startPosition, u8 endPosition, u8 amplitudeA, u8 amplitudeB, u8 frequency, u8 period);

    */

    //JoyShockDeviceConnected& OnJoyShockDeviceConnected();
    //JoyShockDeviceDisconnected& OnJoyShockDeviceDisconnected();
}
