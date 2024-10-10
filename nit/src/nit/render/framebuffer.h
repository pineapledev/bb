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

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;

        FramebufferTextureSpecification(FramebufferTextureFormat format)
            : texture_format(format)
        {
        }

        FramebufferTextureFormat texture_format = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;

        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : attachments(attachments)
        {
        }
        
        Array<FramebufferTextureSpecification> attachments;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0, height = 0;
        FramebufferAttachmentSpecification attachments;
        uint32_t samples = 1;

        bool swap_chain_target = false;
    };

    struct Framebuffer
    {
        void Invalidate();
        void Bind() const;
        void Unbind() const;
        void Resize(uint32_t width, uint32_t height);
        
        int ReadPixel(uint32_t attachment_index, int x, int y);
        void ClearAttachment(uint32_t attachment_index, int value);

        uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const
        {
            NIT_CHECK((index < (uint32_t) color_attachments.size()), "Invalid color attachment!");
            return color_attachments[index];
        }
        
        uint32_t frame_buffer_id = 0;
        FramebufferSpecification specification;
        
        Array<FramebufferTextureSpecification> color_attachment_specifications;
        FramebufferTextureSpecification depth_attachment_specification = FramebufferTextureFormat::None;

        Array<uint32_t> color_attachments;
        uint32_t depth_attachment = 0;
    };

    void LoadFrameBuffer(Framebuffer* framebuffer);
    void FreeFrameBuffer(Framebuffer* framebuffer);
}