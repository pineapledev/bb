#pragma once
#include "entity.h"

namespace Nit
{
    struct Scene
    {
        String cached_scene;
        Array<Entity> entities;
    };
    
    void RegisterSceneAsset();
    void SerializeScene(const Scene* scene, YAML::Emitter& emitter);
    void DeserializeScene(Scene* scene, const YAML::Node& node);
    void LoadScene(Scene* scene);
    void FreeScene(Scene* scene);
    bool AreSceneEntitiesLoaded(const Scene* scene);

    void SaveSceneEntities(Scene* scene);
    void FreeSceneEntities(Scene* scene);
    void LoadSceneEntities(Scene* scene);
}
