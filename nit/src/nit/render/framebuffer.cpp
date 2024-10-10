#include "framebuffer.h"
#include "glad/glad.h"

namespace Nit
{
    static constexpr uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

    GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    void CreateTextures(bool multisampled, uint32_t* out_id, uint32_t count)
    {
        glCreateTextures(TextureTarget(multisampled), count, out_id);
    }

    void BindTexture(bool multisampled, uint32_t id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }

    void AttachColorTexture(uint32_t id, uint32_t samples, GLenum internal_format, GLenum format, uint32_t width,
                            uint32_t height, int index)
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

    void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachment_type, uint32_t width,
                            uint32_t height)
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

    bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format)
        {
        case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
        }

        return false;
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
            glDeleteTextures((GLsizei)color_attachments.size(), color_attachments.data());
            glDeleteTextures(1, &depth_attachment);

            color_attachments.clear();
            depth_attachment = 0;
        }

        glCreateFramebuffers(1, &frame_buffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

        bool multisample = specification.samples > 1;

        // Attachments
        if (!color_attachment_specifications.empty())
        {
            color_attachments.resize(color_attachment_specifications.size());
            CreateTextures(multisample, color_attachments.data(), (uint32_t)color_attachments.size());

            for (size_t i = 0; i < color_attachments.size(); i++)
            {
                BindTexture(multisample, color_attachments[i]);
                switch (color_attachment_specifications[i].texture_format)
                {
                case FramebufferTextureFormat::RGBA8:
                    AttachColorTexture(color_attachments[i], specification.samples, GL_RGBA8, GL_RGBA,
                                       specification.width, specification.height, (int)i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    AttachColorTexture(color_attachments[i], specification.samples, GL_R32I, GL_RED_INTEGER,
                                       specification.width, specification.height, (int)i);
                    break;
                }
            }
        }

        if (depth_attachment_specification.texture_format != FramebufferTextureFormat::None)
        {
            CreateTextures(multisample, &depth_attachment, 1);
            BindTexture(multisample, depth_attachment);
            switch (depth_attachment_specification.texture_format)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                AttachDepthTexture(depth_attachment, specification.samples, GL_DEPTH24_STENCIL8,
                                   GL_DEPTH_STENCIL_ATTACHMENT, specification.width, specification.height);
                break;
            }
        }

        if (color_attachments.size() > 1)
        {
            NIT_CHECK((color_attachments.size() <= 4), "Invalid size!");
            GLenum buffers[4] = {
                GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
            };
            glDrawBuffers((GLsizei)color_attachments.size(), buffers);
        }
        else if (color_attachments.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        NIT_CHECK((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
        glViewport(0, 0, specification.width, specification.height);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
        {
            NIT_LOG_WARN("Attempted to resize Framebuffer to %i, %i", width, height);
            return;
        }
        specification.width = width;
        specification.height = height;

        Invalidate();
    }

    int Framebuffer::ReadPixel(uint32_t attachment_index, int x, int y)
    {
        NIT_CHECK((attachment_index < color_attachments.size()), "Invalid index!");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
        int pixel_data;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
        return pixel_data;
    }

    void Framebuffer::ClearAttachment(uint32_t attachment_index, int value)
    {
        NIT_CHECK((attachment_index < color_attachments.size()), "Invalid index!");

        auto& spec = color_attachment_specifications[attachment_index];
        glClearTexImage(color_attachments[attachment_index], 0,
                        FbTextureFormatToGL(spec.texture_format), GL_INT, &value);
    }

    void LoadFrameBuffer(Framebuffer* framebuffer)
    {
        NIT_CHECK(framebuffer);
        for (auto texture_specification : framebuffer->specification.attachments.attachments)
        {
            if (!IsDepthFormat(texture_specification.texture_format))
            {
                framebuffer->color_attachment_specifications.emplace_back(texture_specification);
            }
            else
            {
                framebuffer->depth_attachment_specification = texture_specification;
            }
        }

        framebuffer->Invalidate();
    }

    void FreeFrameBuffer(Framebuffer* framebuffer)
    {
        NIT_CHECK(framebuffer);
        glDeleteFramebuffers(1, &framebuffer->frame_buffer_id);
        framebuffer->frame_buffer_id = 0;
        glDeleteTextures(((GLsizei)framebuffer->color_attachments.size()), framebuffer->color_attachments.data());
        glDeleteTextures(1, &framebuffer->depth_attachment);
    }
}
