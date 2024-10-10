#pragma once

namespace Nit
{
    enum class FramebufferTextureFormat : u8
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

    struct Framebuffer
    {
        void Invalidate();
        void Bind();
        void Unbind();
        void Resize(u32 new_width, u32 new_height);
        
        int ReadPixel(u32 attachment_index, int x, int y);
        void ClearAttachment(u32 attachment_index, int value);

        u32 GetColorAttachmentID(u32 index = 0)
        {
            NIT_CHECK((index < (u32) color_attachment_ids.size()), "Invalid color attachment!");
            return color_attachment_ids[index];
        }

        u32                             width                = 0;
        u32                             height               = 0;
        u32                             samples              = 1;
        u32                             frame_buffer_id      = 0;
        Array<FramebufferTextureFormat> color_attachments    = {};
        FramebufferTextureFormat        depth_attachment     = FramebufferTextureFormat::None;
        Array<u32>                      color_attachment_ids = {};
        u32                             depth_attachment_id  = 0;
    };

    void LoadFrameBuffer(Framebuffer* framebuffer);
    void FreeFrameBuffer(Framebuffer* framebuffer);
}