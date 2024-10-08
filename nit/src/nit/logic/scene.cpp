#include "scene.h"

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