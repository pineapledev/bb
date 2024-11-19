#include "uuid.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    bool uuid_valid(const UUID& uuid)
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

    void serialize(const UUID* uuid, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "data" << YAML::Value << uuid->data;
    }

    void deserialize(UUID* uuid, const YAML::Node& node)
    {
        uuid->data = node["data"].as<u64>();
    }
    
#ifdef NIT_EDITOR_ENABLED
    void draw_editor(UUID* uuid)
    {
        editor_draw_text("data", "%llu", uuid->data);
        ImGui::Spacing();
    }
#endif

    ListenerAction on_component_added(const ComponentAddedArgs& args)
    {
        if (args.type != type_get<UUID>())
        {
            return ListenerAction::StayListening;
        }

        auto& uuid = entity_get<UUID>(args.entity);

        if (uuid_valid(uuid))
        {
            return ListenerAction::StayListening;
        }

        uuid = uuid_generate();

        return ListenerAction::StayListening;
    }
    
    void register_uuid_component()
    {
        component_register<UUID>({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });

        entity_create_group<UUID>();

        entity_registry_get_instance()->component_added_event += ComponentAddedListener::create(on_component_added);
    }
}