#pragma once


#define HOMING_MISSILE_GROUP_SIGNATURE Transform, Sprite, HomingMissile, Rigidbody2D, CircleCollider, Hittable, Health
#define ENTITY_NAME_HOMING_MISSILE_PRESET "HomingMissilePreset"

struct HomingMissile
{
    bool enabled    = true;
    bool attacking  = false;
    Vector3 attack_dir = V3_ONE;
    f32  wait_timer = 0.f;
    // Asset
    f32  distance_to_attack = 1.f;
    f32  wait_time          = 0.5f;
    f32  approach_speed     = 3.f;
    f32  attack_speed       = 6.f;
};

void register_homing_missile_component();

void homing_missile_start();
void homing_missile_update();
