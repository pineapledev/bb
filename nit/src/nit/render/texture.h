#pragma once

namespace Nit
{
    struct Image
    {
        Image();
        Image(const char* file_path);
        Image(const Image& other) = delete;
        Image(Image&& other) noexcept;
        ~Image();

        Image& operator=(const Image& other) = delete;
        Image& operator=(Image&& other) noexcept;

        void Load(const char* file_path);
        void Free();
        
        unsigned char* data = nullptr;
        u32 width = 0;
        u32 height = 0;
        u32 channels = 0;
    };

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

    struct Texture2DCfg
    {
        MagFilter mag_filter = MagFilter::Linear;
        MinFilter min_filter = MinFilter::Linear;
        WrapMode  wrap_mode_u = WrapMode::Repeat;
        WrapMode  wrap_mode_v = WrapMode::Repeat;
    };

    struct Texture2D
    {
        Texture2D();
        Texture2D(const void* data, u32 width, u32 height, u32 channels, const Texture2DCfg& cfg = {});
        Texture2D(const Image& image, const Texture2DCfg& cfg = {});
        ~Texture2D();
        
        void UploadToGPU(const void* data, u32 width, u32 height, u32 channels, const Texture2DCfg& cfg = {});
        void UploadToGPU(const Image& image, const Texture2DCfg& cfg = {});
        
        void Bind(u32 slot = 0) const;
        
        Vector2 size     = V2_ZERO;
        u32     id       = 0;
        bool    uploaded = false;
    };
}