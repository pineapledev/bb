#pragma once

namespace Nit
{
    enum class FrameBufferTextureFormat : u8
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FrameBuffer
    {
        u32                             width                = 0;
        u32                             height               = 0;
        u32                             samples              = 1;
        u32                             frame_buffer_id      = 0;
        Array<FrameBufferTextureFormat> color_attachments    = {};
        FrameBufferTextureFormat        depth_attachment     = FrameBufferTextureFormat::None;
        Array<u32>                      color_attachment_ids = {};
        u32                             depth_attachment_id  = 0;
    };

    void LoadFrameBuffer(FrameBuffer* framebuffer);
    
    void FreeFrameBuffer(FrameBuffer* framebuffer);
    
    void ResizeFrameBuffer(FrameBuffer* framebuffer, u32 new_width, u32 new_height);
    
    i32  ReadFrameBufferPixel(const FrameBuffer* framebuffer, u32 attachment_index, i32 x, i32 y);
    
    void ClearAttachment(const FrameBuffer* framebuffer, u32 attachment_index, i32 value);
    
    void BindFrameBuffer(const FrameBuffer* framebuffer);
    
    void UnbindFrameBuffer();
}