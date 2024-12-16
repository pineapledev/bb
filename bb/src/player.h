#pragma once

struct Player
{
    nit::f32 firerate_gun_1 = 1.0f;
    nit::f32 firerate_gun_2 = 1.0f;
    nit::f32 firerate_gun_3 = 1.0f;
    nit::f32 firerate_gun_max = 1.0f;
    nit::f32 firerate_sword_1 = 1.0f;
    nit::f32 firerate_sword_2 = 1.0f;
    nit::f32 firerate_sword_3 = 1.0f;
    nit::f32 firerate_sword_max = 1.0f; 
};

void register_player_component();