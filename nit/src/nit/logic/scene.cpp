#include "scene.h"
#include "core/app.h"
#include "nit/core/asset.h"

namespace Nit
{
    void RegisterSceneAsset()
    {
        RegisterAssetType<Scene>({
              LoadScene
            , FreeScene
            , SerializeScene
            , DeserializeScene
        });
    }

    void SerializeSceneEntities(const Scene* scene, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginMap;
        for (Entity entity : scene->entities)
        {
            SerializeEntity(entity, emitter);
        }
        emitter << YAML::EndMap;
    }
    
    void SerializeScene(const Scene* scene, YAML::Emitter& emitter)
    {
        // More items to serialize
        SerializeSceneEntities(scene, emitter);
    }

    void DeserializeScene(Scene* scene, const YAML::Node& node)
    {
        StringStream ss;
        ss << node;
        scene->cached_scene = ss.str();
    }
    
    void LoadScene(Scene* scene)
    {
        LoadSceneEntities(scene);
        scene->loaded = true;
    }

    void FreeScene(Scene* scene)
    {
        FreeSceneEntities(scene);
        scene->loaded = false;
    }

    bool AreSceneEntitiesLoaded(const Scene* scene)
    {
        return !scene->entities.empty();
    }

    void SaveSceneEntities(Scene* scene)
    {
        NIT_CHECK(scene);
        YAML::Emitter emitter;
        SerializeSceneEntities(scene, emitter);
        scene->cached_scene = emitter.c_str();
    }

    void FreeSceneEntities(Scene* scene)
    {
        NIT_CHECK(scene);
        if (!scene->entities.empty())
        {
            for (Entity entity : scene->entities)
            {
                DestroyEntity(entity);
            }
        }
        scene->entities.clear();
    }

    void LoadSceneEntities(Scene* scene)
    {
        NIT_CHECK(scene);
        FreeSceneEntities(scene);
        const YAML::Node node = YAML::Load(scene->cached_scene);
        const YAML::Node& entities_node = node["Entities"];

        for (const auto& entity_node : entities_node)
        {
            const YAML::Node& entity_node_value = entity_node.second;
            Entity entity = DeserializeEntity(entity_node_value);
            scene->entities.push_back(entity);
        }
    }
}
