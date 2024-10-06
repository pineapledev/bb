#pragma once

namespace Nit
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
    
    struct Texture2D
    {
        u32          id                = 0;
        bool         loaded_from_image = false;
        bool         keep_pixel_data   = false;
        bool         is_white_texture  = false;
        u8*          pixel_data        = nullptr;
        Vector2      size              = V2_ZERO;
        u32          width             = 0;
        u32          height            = 0;
        u32          channels          = 0;
        String       image_path;       
        MagFilter    mag_filter        = MagFilter::Linear;
        MinFilter    min_filter        = MinFilter::Linear;
        WrapMode     wrap_mode_u       = WrapMode::Repeat;
        WrapMode     wrap_mode_v       = WrapMode::Repeat;
    };

    void RegisterTexture2DAsset();
    void SerializeTexture2D(const Texture2D* texture, YAML::Emitter& emitter);
    void DeserializeTexture2D(Texture2D* texture, const YAML::Node& node);
    void LoadTexture2D(Texture2D* texture);
    void FreeTexture2D(Texture2D* texture);
    void BindTexture2D(const Texture2D& texture, u32 slot = 0);
}