#pragma once

#define ENTITY_NAME_PLAYER                "Player"
#define INPUT_NAME_PLAYER_MOVEMENT        "PlayerMoveInput"
#define INPUT_NAME_PLAYER_MOVEMENT_LEFT   "PlayerMoveLeftInput"
#define INPUT_NAME_PLAYER_MOVEMENT_RIGHT  "PlayerMoveRightInput"
#define INPUT_NAME_PLAYER_MOVEMENT_UP     "PlayerMoveUpInput"
#define INPUT_NAME_PLAYER_MOVEMENT_DOWN   "PlayerMoveDownInput"
#define INPUT_NAME_PLAYER_SHOOT           "PlayerShootInput"

struct Player
{
    AssetHandle input_move  = {};
    AssetHandle input_move_left  = {};
    AssetHandle input_move_right  = {};
    AssetHandle input_move_up  = {};
    AssetHandle input_move_down  = {};
    AssetHandle input_shoot = {};
    
    f32 firerate_gun_1     = 1.0f;
    f32 firerate_gun_2     = 1.0f;
    f32 firerate_gun_3     = 1.0f;
    f32 firerate_gun_max   = 1.0f;
    f32 firerate_sword_1   = 1.0f;
    f32 firerate_sword_2   = 1.0f;
    f32 firerate_sword_3   = 1.0f;
    f32 firerate_sword_max = 1.0f; 
};

void register_player_component();

void player_start();
void player_update();
