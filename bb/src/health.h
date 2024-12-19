#pragma once

struct Health
{
    f32 max = 1.0f;
    f32 current = max;
};

void register_health_component();
