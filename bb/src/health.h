#pragma once

struct Health
{
    f32 max = 1.0f;
    f32 current = max;
};

struct Hittable
{
};

void register_health_component();
void register_hittable_component();
void hittable_start();
