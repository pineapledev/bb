#pragma once

#include "nit.h"
#include "health.h"
#include "movement.h"
#include "player.h"

//TODO: remove nit namespace from engine
using AssetHandle    = nit::AssetHandle;
using EntityID       = nit::EntityID;
using ListenerAction = nit::ListenerAction;
using InputListener  = nit::Listener<const nit::InputActionContext&>;

#define ENTITY_NAME_PLAYER          "Player"
#define SCENE_NAME_GAME             "GameScene"
#define INPUT_NAME_PLAYER_MOVEMENT  "PlayerMoveInput"

struct Game
{
    EntityID    player            = nit::NULL_ENTITY;
    AssetHandle test_scene        = {};
    AssetHandle player_input_move = {};
};

inline Game* game = nullptr;

ListenerAction game_start();
ListenerAction game_update();