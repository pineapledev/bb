#pragma once
#include "input_enums.h"
#include "input_keycodes.h"
#include "input_action.h"

namespace nit
{
    struct AssetPool; 

    struct InputModifierPool
    {
        Pool pool;
        Type* input_modifier_type = nullptr;
    };

    struct InputRegistry
    {
        AssetPool* input_actions = nullptr;
        Array<InputModifierPool> input_modifiers;
    };

    

    void input_registry_set_instance(InputRegistry* input_registry_instance); 
    InputRegistry* input_registry_get_instance();
    void input_registry_init();


    // ------------------------
    // INPUT SYSTEM
    // ------------------------

    //void SendForceFeedback(i32 _controllerId, f32 _leftMotor, f32 _rightMotor);

    // ------------------------
    // INPUT
    // ------------------------

    bool IsKeyPressed(u32 key);
    void GetButtonState(GamepadKeys key, InputContent& input_content);
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
