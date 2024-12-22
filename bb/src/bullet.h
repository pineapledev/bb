#pragma once

struct Bullet
{
    bool    enabled = true;
    Vector3 dir     = V3_UP;
    f32     damage  = 10.f;
};

#define BULLET_GROUP_SIGNATURE Transform, Sprite, Movement, Bullet, Rigidbody2D, CircleCollider
#define ENTITY_NAME_BULLET_PRESET "BulletPreset"

void register_bullet_component();

EntityID bullet_spawn(const Vector3& pos, const Vector3& dir = V3_UP);
void     bullet_start();
void     bullet_update();