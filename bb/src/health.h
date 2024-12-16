#pragma once

struct Health
{
    nit::f32 max = 1.0f;
    nit::f32 current = max;
};

void register_health_component();
