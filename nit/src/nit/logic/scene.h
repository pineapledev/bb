﻿#pragma once
#include "entity.h"

namespace Nit
{
    struct Scene
    {
        Array<Entity> entities;
    };
    
    void RegisterSceneAsset();
    void SerializeScene(const Scene* scene, YAML::Emitter& emitter);
    void DeserializeScene(Scene* scene, const YAML::Node& node);
    void LoadScene(Scene* scene);
    void FreeScene(Scene* scene);
    bool IsSceneLoaded(const Scene* scene);
}