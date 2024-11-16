#include "nit_pch.h"
#include "sprite.h"
#include "texture.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
{
    void SerializeSprite(const Sprite* sprite, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "texture"       << YAML::Value << sprite->texture;
        emitter << YAML::Key << "sub_texture"   << YAML::Value << sprite->sub_texture;
        emitter << YAML::Key << "visible"       << YAML::Value << sprite->visible;
        emitter << YAML::Key << "tint"          << YAML::Value << sprite->tint;
        emitter << YAML::Key << "size"          << YAML::Value << sprite->size;
        emitter << YAML::Key << "flip_x"        << YAML::Value << sprite->flip_x;
        emitter << YAML::Key << "flip_y"        << YAML::Value << sprite->flip_y;
        emitter << YAML::Key << "tiling_factor" << YAML::Value << sprite->tiling_factor;
        emitter << YAML::Key << "keep_aspect"   << YAML::Value << sprite->keep_aspect;
    }
    
    void DeserializeSprite(Sprite* sprite, const YAML::Node& node)
    {
        sprite->texture       = node["texture"]       .as<AssetHandle>();
        sprite->sub_texture   = node["sub_texture"]   .as<String>();
        sprite->visible       = node["visible"]       .as<bool>();
        sprite->tint          = node["tint"]          .as<Vector4>();
        sprite->size          = node["size"]          .as<Vector2>();
        sprite->flip_x        = node["flip_x"]        .as<bool>();
        sprite->flip_y        = node["flip_y"]        .as<bool>();
        sprite->tiling_factor = node["tiling_factor"] .as<Vector2>();
        sprite->keep_aspect   = node["keep_aspect"]   .as<bool>();
    }

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorSprite(Sprite* sprite)
    {
        editor_draw_asset_combo("texture", GetType<Texture2D>(), &sprite->texture);
        
        editor_draw_input_text("sub_texture", sprite->sub_texture);

        if (asset_valid(sprite->texture))
        {
            SetSpriteSubTexture2D(*sprite, sprite->sub_texture);
        }
        else
        {
            sprite->sub_texture_index = -1;
        }
        
        editor_draw_bool("visible", sprite->visible);
        editor_draw_color_palette("tint", sprite->tint);
        editor_draw_drag_vector2("size", sprite->size);
        editor_draw_bool("flip_x", sprite->flip_x);
        editor_draw_bool("flip_y", sprite->flip_y);
        editor_draw_drag_vector2("tiling_factor", sprite->tiling_factor);
        editor_draw_bool("keep_aspect", sprite->keep_aspect);
    }
#endif

    void register_sprite_component()
    {
        TypeArgs<Sprite> args;
        args.fn_serialize   = SerializeSprite;
        args.fn_deserialize = DeserializeSprite;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorSprite;
#endif
        type_register<Sprite>(args);
        component_register<Sprite>();
    }

    void SetSpriteSubTexture2D(Sprite& sprite, const String& sub_texture)
    {
        sprite.sub_texture = sub_texture;
        
        if (!asset_valid(sprite.texture))
        {
            sprite.sub_texture_index = -1;
            return;
        }

        i32 index = texture_2d_get_sub_tex_index(asset_get_data<Texture2D>(sprite.texture), sub_texture);
        sprite.sub_texture_index = index;
    }

    void ResetSpriteSubTexture2D(Sprite& sprite)
    {
        sprite.sub_texture = "";
        sprite.sub_texture_index = -1;
    }
}