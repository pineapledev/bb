#include "scene.h"

#include "core/app.h"

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

    void SerializeScene(const Scene* scene, YAML::Emitter& emitter)
    {
        for(Entity entity : scene->entities)
        {
            emitter << YAML::Key << "Entity" << YAML::Value << YAML::BeginMap;
            
            for (u32 i = 0; i < app->entity_registry.next_component_type_index - 1; ++i)
            {
                auto& component_pool = app->entity_registry.component_pool[i];
                auto& data_pool = component_pool.data_pool;
            
                if (!data_pool.type->fn_deserialize
                    || !data_pool.type->fn_serialize
                    || !Invoke(component_pool.fn_is_in_entity, entity))
                {
                    continue;
                }

                emitter << YAML::Key << data_pool.type->name << YAML::Value << YAML::BeginMap;
                
                void* raw_data = GetPoolElementRawPtr(&data_pool, entity);
                SerializeRawData(data_pool.type, raw_data, emitter);
                
                emitter << YAML::EndMap;
            }

            emitter << YAML::EndMap;
        }
    }

    void DeserializeScene(Scene* scene, const YAML::Node& node)
    {
    }
    
    void LoadScene(Scene* scene)
    {
    }

    void FreeScene(Scene* scene)
    {
    }

    bool IsSceneLoaded(const Scene* scene)
    {
        return false;
    }
}