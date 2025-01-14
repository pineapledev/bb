#include "homing_missile.h"

#include "game.h"
#include "hittable.h"
#include "health.h"

using namespace nit;

void serialize(const HomingMissile* homing_missile, YAML::Emitter& emitter)
{
    emitter << YAML::Key << "distance_to_attack" << YAML::Value << homing_missile->distance_to_attack;
    emitter << YAML::Key << "wait_time"          << YAML::Value << homing_missile->wait_time;
    emitter << YAML::Key << "approach_speed"     << YAML::Value << homing_missile->approach_speed;
    emitter << YAML::Key << "attack_speed"       << YAML::Value << homing_missile->attack_speed;
}

void deserialize(HomingMissile* homing_missile, const YAML::Node& node)
{
    homing_missile->distance_to_attack = node["distance_to_attack"].as<float>();
    homing_missile->wait_time          = node["wait_time"].as<float>();
    homing_missile->approach_speed     = node["approach_speed"].as<float>();
    homing_missile->attack_speed       = node["attack_speed"].as<float>();
}

#ifdef NIT_EDITOR_ENABLED
void draw_editor(HomingMissile* homing_missile)
{
    editor_draw_text("Enabled", "%s", homing_missile->enabled ? "True" : "False");
    editor_draw_text("Attacking", "%s", homing_missile->attacking ? "True" : "False");
    editor_draw_text("Attack Direction", "x: %f y: %f, z: %f", homing_missile->attack_dir.x, homing_missile->attack_dir.y, homing_missile->attack_dir.z);
    editor_draw_text("Wait Timer", "%f", homing_missile->wait_timer);
    editor_draw_drag_f32("Distance to Attack", homing_missile->distance_to_attack);
    editor_draw_drag_f32("Wait Time to Attack", homing_missile->wait_time);
    editor_draw_drag_f32("Approach Speed", homing_missile->approach_speed);
    editor_draw_drag_f32("Attack Speed", homing_missile->attack_speed);
}
#endif

void register_homing_missile_component()
{
    TypeArgs<HomingMissile> args;
    args.fn_serialize = serialize;
    args.fn_deserialize = deserialize;
    args.fn_draw_editor = draw_editor;
    component_register<HomingMissile>(args);

    entity_create_group<HOMING_MISSILE_GROUP_SIGNATURE>();
}

void homing_missile_start()
{

}

void homing_missile_update()
{
    for (EntityID entity : entity_get_group<HOMING_MISSILE_GROUP_SIGNATURE>().entities)
    {
        auto& homing_missile = entity_get<HomingMissile>(entity);

        if (!homing_missile.enabled)
        {
            continue;
        }

        auto& transform = entity_get<Transform>(entity);
        auto& player_transform = entity_get<Transform>(game->entity_player);
        float distance = magnitude(player_transform.position - transform.position);
        Vector3 dir = (player_transform.position - transform.position) / distance;

        if(distance <= homing_missile.distance_to_attack)
        {
            homing_missile.attacking = true;
        }
        // Approach ship
        if(!homing_missile.attacking)
        {
            transform.position += dir * homing_missile.approach_speed * delta_seconds();
        }
        else
        {
            // Waiting to attack
            if(homing_missile.wait_timer < homing_missile.wait_time)
            {
                homing_missile.wait_timer += delta_seconds();
                if(homing_missile.wait_timer >= homing_missile.wait_time)
                {
                    homing_missile.attack_dir = normalize(player_transform.position - transform.position);
                }
            }
            // Attack!
            else
            {
                transform.position += homing_missile.attack_dir * homing_missile.attack_speed * delta_seconds();
                // Explode after a set time?
            }
        }
    }

}

