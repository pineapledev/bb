#pragma once

#define SCENE_NAME_GAME    "GameScene"

struct Game
{
    EntityID    entity_player  = NULL_ENTITY;
    AssetHandle test_scene     = {};
};

inline Game* game = nullptr;

ListenerAction game_start();
ListenerAction game_update();