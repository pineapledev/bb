﻿#include "game.h"

#include "bullet.h"
#include "player.h"

ListenerAction game_start()
{
    // Scene assets
    {
        game->test_scene = asset_find_by_name(SCENE_NAME_GAME);
    
        if (asset_valid(game->test_scene))
        {
            asset_load(game->test_scene);
        }
    }
    
    // Entities
    {
        game->entity_player = entity_find_by_name(ENTITY_NAME_PLAYER);

        if (entity_valid(game->entity_player))
        {
            player_start();    
        }

        game->entity_bullet_preset = entity_find_by_name(ENTITY_NAME_BULLET_PRESET);

        if (entity_valid(game->entity_bullet_preset))
        {
            bullet_start();
        }
    }
    
    return ListenerAction::StayListening;
}

ListenerAction game_update()
{
    if (!entity_valid(game->entity_player))
    {
        NIT_CHECK(false);
        return ListenerAction::StopListening;
    }

    player_update();
    bullet_update();
    return ListenerAction::StayListening;
}