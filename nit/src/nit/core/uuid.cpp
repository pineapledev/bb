#include "uuid.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    bool IsValid(const UUID& uuid)
    {
        return uuid.data != 0;
    }

    bool operator==(const UUID& a, const UUID& b)
    {
        return a.data == b.data;
    }

    bool operator!=(const UUID& a, const UUID& b)
    {
        return !(a == b);
    }

    void SerializeUUID(const UUID* uuid, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "data" << YAML::Value << uuid->data;
    }

    void DeserializeUUID(UUID* uuid, const YAML::Node& node)
    {
        uuid->data = node["data"].as<u64>();
    }
    
#ifdef NIT_EDITOR_ENABLED
    void DrawEditorUUID(UUID* uuid)
    {
        editor_draw_text("data", "%llu", uuid->data);
        ImGui::Spacing();
    }
#endif
    
    void register_uuid_component()
    {
        component_register<UUID>
        ({
            .fn_serialize   = SerializeUUID,
            .fn_deserialize = DeserializeUUID,
#ifdef NIT_EDITOR_ENABLED
            .fn_draw_editor = DrawEditorUUID
#endif
        });

        entity_create_group<UUID>();
    }
}