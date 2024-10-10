#include "framebuffer.h"
#include "glad/glad.h"

#ifdef NIT_GRAPHICS_API_OPENGL

namespace Nit
{
    static constexpr u32 MAX_FRAMEBUFFER_SIZE = 8192;

    GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    void CreateTextures(bool multisampled, u32* out_id, u32 count)
    {
        glCreateTextures(TextureTarget(multisampled), count, out_id);
    }

    void BindTexture(bool multisampled, u32 id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }

    void AttachColorTexture(u32 id, u32 samples, GLenum internal_format, GLenum format, u32 width, u32 height, i32 index)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
    }

    void AttachDepthTexture(u32 id, i32 samples, GLenum format, GLenum attachment_type, u32 width, u32 height)
    {
        bool multisampled = samples > 1;
        
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, TextureTarget(multisampled), id, 0);
    }

    GLenum FbTextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format)
        {
        case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
        case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
        }

        NIT_CHECK(false, "Invalid format!");
        return 0;
    }

    void Framebuffer::Invalidate()
    {
        if (frame_buffer_id)
        {
            glDeleteFramebuffers(1, &frame_buffer_id);
            glDeleteTextures((GLsizei)color_attachment_ids.size(), color_attachment_ids.data());
            glDeleteTextures(1, &depth_attachment_id);

            color_attachment_ids.clear();
            depth_attachment_id = 0;
        }

        glCreateFramebuffers(1, &frame_buffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

        bool multisample = samples > 1;

        // Attachments
        if (!color_attachments.empty())
        {
            color_attachment_ids.resize(color_attachments.size());
            CreateTextures(multisample, color_attachment_ids.data(), (u32)color_attachment_ids.size());

            for (size_t i = 0; i < color_attachment_ids.size(); i++)
            {
                BindTexture(multisample, color_attachment_ids[i]);
                switch (color_attachments[i])
                {
                case FramebufferTextureFormat::RGBA8:
                    AttachColorTexture(color_attachment_ids[i], samples, GL_RGBA8, GL_RGBA,
                                       width, height, (i32)i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    AttachColorTexture(color_attachment_ids[i], samples, GL_R32I, GL_RED_INTEGER,
                                       width, height, (i32)i);
                    break;
                }
            }
        }

        if (depth_attachment != FramebufferTextureFormat::None)
        {
            CreateTextures(multisample, &depth_attachment_id, 1);
            BindTexture(multisample, depth_attachment_id);
            switch (depth_attachment)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                AttachDepthTexture(depth_attachment_id, samples, GL_DEPTH24_STENCIL8,
                                   GL_DEPTH_STENCIL_ATTACHMENT, width, height);
                break;
            }
        }

        if (color_attachment_ids.size() > 1)
        {
            NIT_CHECK((color_attachment_ids.size() <= 4), "Invalid size!");
            GLenum buffers[4] = {
                GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
            };
            glDrawBuffers((GLsizei)color_attachment_ids.size(), buffers);
        }
        else if (color_attachment_ids.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        NIT_CHECK((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
        glViewport(0, 0, width, height);
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(u32 new_width, u32 new_height)
    {
        if (new_width == 0 || new_height == 0 || new_width > MAX_FRAMEBUFFER_SIZE || new_height > MAX_FRAMEBUFFER_SIZE)
        {
            NIT_LOG_WARN("Attempted to resize Framebuffer to %i, %i", new_width, new_height);
            return;
        }

        width = new_width;
        height = new_height;

        Invalidate();
    }

    i32 Framebuffer::ReadPixel(u32 attachment_index, i32 x, i32 y)
    {
        NIT_CHECK((attachment_index < color_attachment_ids.size()), "Invalid index!");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        i32 pixel_data;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
        return pixel_data;
    }

    void Framebuffer::ClearAttachment(u32 attachment_index, i32 value)
    {
        NIT_CHECK((attachment_index < color_attachment_ids.size()), "Invalid index!");

        auto& spec = color_attachments[attachment_index];
        glClearTexImage(color_attachment_ids[attachment_index], 0,
                        FbTextureFormatToGL(spec), GL_INT, &value);
    }

    void LoadFrameBuffer(Framebuffer* framebuffer)
    {
        framebuffer->Invalidate();
    }

    void FreeFrameBuffer(Framebuffer* framebuffer)
    {
        NIT_CHECK(framebuffer);
        glDeleteFramebuffers(1, &framebuffer->frame_buffer_id);
        framebuffer->frame_buffer_id = 0;
        glDeleteTextures(((GLsizei)framebuffer->color_attachment_ids.size()), framebuffer->color_attachment_ids.data());
        glDeleteTextures(1, &framebuffer->depth_attachment_id);
    }
}

#endif