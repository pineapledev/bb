#pragma once

#define SCENE_NAME_GAME    "GameScene"
#define SCENE_NAME_PRESETS "PresetsScene"

struct Game
{
    EntityID    entity_player        = NULL_ENTITY;
    EntityID    entity_bullet_preset = NULL_ENTITY;
    AssetHandle scene_game           = {};
    AssetHandle scene_presets        = {};
};

inline Game* game = nullptr;

ListenerAction game_start();
ListenerAction game_update();