#pragma once

struct Bullet
{
    bool enabled = true;
};

#define BULLET_GROUP_SIGNATURE Transform, Sprite, Movement, Bullet, Rigidbody2D, CircleCollider
#define ENTITY_NAME_BULLET_PRESET "BulletPreset"

void register_bullet_component();

EntityID bullet_spawn(const Vector3& pos);
void     bullet_start();
void     bullet_update();