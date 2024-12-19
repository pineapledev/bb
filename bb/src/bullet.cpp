#include "bullet.h"
#include "game.h"
#include "movement.h"

void serialize(const Bullet* bullet, YAML::Emitter& emitter)
{
}

void deserialize(Bullet* bullet, const YAML::Node& node)
{
}

void register_bullet_component()
{
    component_register<Bullet>({
        .fn_serialize = serialize,
        .fn_deserialize = deserialize
    });
    
    entity_create_group<BULLET_GROUP_SIGNATURE>();
}

EntityID bullet_spawn(const Vector3& pos, const Vector3& dir)
{
    if (!entity_valid(game->entity_bullet_preset))
    {
        NIT_CHECK(false);
        return NULL_ENTITY;
    }
    
    EntityID instance = entity_clone(game->entity_bullet_preset);

    auto& bullet  = entity_get<Bullet>(instance);
    bullet.dir = dir;
    
    auto& rb = entity_get<Rigidbody2D>(instance);
    auto& sprite = entity_get<Sprite>(instance);
    auto& transform = entity_get<Transform>(instance);
    
    transform.position  = pos;
    bullet.enabled      = true;
    rb.enabled          = true;
    rb.follow_transform = true;
    sprite.visible      = true;
    
    return instance;
}

void bullet_start()
{
    auto& rb = entity_get<Rigidbody2D>(game->entity_bullet_preset);
    auto& sprite = entity_get<Sprite>(game->entity_bullet_preset);
    auto& bullet  = entity_get<Bullet>(game->entity_bullet_preset);

    rb.enabled     = false;
    sprite.visible = false;
    bullet.enabled = false;
}

void bullet_update()
{
    for (EntityID entity : entity_get_group<BULLET_GROUP_SIGNATURE>().entities)
    {
        auto& bullet  = entity_get<Bullet>(entity);

        if (!bullet.enabled)
        {
            continue;
        }
        
        auto& movement  = entity_get<Movement>(entity);
        auto& transform = entity_get<Transform>(entity);
        
        transform.position += to_v3(multiply(movement.speed, to_v2(bullet.dir)) * delta_seconds());
    }
}
