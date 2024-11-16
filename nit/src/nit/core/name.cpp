#include "name.h"

#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    bool name_empty(const Name& name)
    {
        return name.data.empty();
    }

    bool operator==(const Name& a, const Name& b)
    {
        return a.data == b.data;
    }

    bool operator!=(const Name& a, const Name& b)
    {
        return !(a == b);
    }

    bool name_contains(const Name& name, const String& other)
    {
        return name.data.find(other) != String::npos;
    }

    void serialize(const Name* name, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "data" << YAML::Value << name->data;
    }

    void deserialize(Name* name, const YAML::Node& node)
    {
        name->data = node["data"].as<String>();
    }
    
#ifdef NIT_EDITOR_ENABLED
    void draw_editor(Name* name)
    {
        editor_draw_input_text("data", name->data);
        ImGui::Spacing();
    }
#endif

    void register_name_component()
    {
        component_register<Name>
        ({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });

        entity_create_group<Name>();
    }
}
