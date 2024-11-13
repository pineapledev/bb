#pragma once

namespace nit
{
    enum class MinFilter : u8
    {
        Linear, Nearest
    };

    enum class MagFilter : u8
    {
        Linear, Nearest
    };

    enum class WrapMode : u8
    {
        Repeat, ClampToEdge
    };

    enum class TextureCoordinate : u8
    {
        U, V
    };
    
    struct SubTexture2D
    {
        String  name;
        Vector2 size;
        Vector2 location;
    };
    
    void DeserializeSubTexture2D(SubTexture2D* sub_texture, const YAML::Node& node);
    void SerializeSubTexture2D(const SubTexture2D* sub_texture, YAML::Emitter& emitter);
    
    struct Texture2D
    {
        u32           id                = 0;
        u8*           pixel_data        = nullptr;
        Vector2       size;
        u32           channels          = 0;
        String        image_path;       
        MagFilter     mag_filter        = MagFilter::Linear;
        MinFilter     min_filter        = MinFilter::Linear;
        WrapMode      wrap_mode_u       = WrapMode::Repeat;
        WrapMode      wrap_mode_v       = WrapMode::Repeat;
        u32           sub_texture_count = 0;
        SubTexture2D* sub_textures      = nullptr;
    };

    void register_texture_2d();
    i32  find_pool_index_of_sub_texture_2d(const Texture2D* texture, const String& sub_texture_name);
    void serialize_texture_2d(const Texture2D* texture, YAML::Emitter& emitter);
    void deserialize_texture_2d(Texture2D* texture, const YAML::Node& node);
#ifdef NIT_EDITOR_ENABLED
    void draw_editor_texture_2d(Texture2D* texture);
#endif
    void load_texture_2d(Texture2D* texture);
    void upload_to_gpu(Texture2D* texture);
    void free_texture_2d(Texture2D* texture);
    void bind_texture_2d(const Texture2D* texture, u32 slot = 0);
    bool is_texture_2d_valid(const Texture2D* texture);
    void load_texture_2d_as_sprite_sheet(Texture2D* texture, const String& sprite_sheet_name, const String& source_path, const String& dest_path, i32 max_width = 5034);
}