#pragma once

struct Health
{
    bool enabled = true;
    f32  max     = 100.0f;
    f32  current = max;
};

void register_health_component();
