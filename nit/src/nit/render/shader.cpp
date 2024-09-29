#include "shader.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif

namespace Nit
{
    Constant::Constant(const TString& name, ShaderDataType type, i32 size)
        : data(CreateFromShaderDataType(type))
          , name(name)
          , type(type)
          , size(size)
    {
        if (!data)
        {
            return;
        }
        memset(data, 0, GetSizeOfShaderDataType(type));
    }

    Constant::Constant(Constant&& other) noexcept
        : data(other.data)
          , name(std::move(other.name))
          , type(other.type)
          , size(other.size)
    {
        other.data = nullptr;
    }

    Constant& Constant::operator=(Constant&& other) noexcept
    {
        data = other.data;
        name = other.name;
        type = other.type;
        size = other.size;
        other.data = nullptr;
        return *this;
    }

    Constant::~Constant()
    {
        switch (type)
        {
        case ShaderDataType::Float:
            delete float_data;
            return;
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            delete [] float_data;
            return;
        case ShaderDataType::Int:
        case ShaderDataType::Bool:
            delete int_data;
            return;
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Sampler2D:
            delete [] int_data;
            return;
        case ShaderDataType::None:
            NIT_CHECK(false);
        }
    }

    Shader::~Shader()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glDeleteProgram(id);
#endif
    }

    void Shader::Compile(const char* vertex_source, const char* fragment_source)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        if (compiled)
        {
            glDeleteProgram(id);
            compiled = false;
        }

        // Create an empty vertex shader handle
        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to GL
        // Note that String's .c_str is NULL character terminated.
        const GLchar* source = vertex_source;
        glShaderSource(vertex_shader, 1, &source, nullptr);

        // Compile the vertex shader
        glCompileShader(vertex_shader);

        GLint is_compiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            GLint max_length = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

            // The maxLength includes the NULL character
            TArray<GLchar> info_log(max_length);
            glGetShaderInfoLog(vertex_shader, max_length, &max_length, info_log.data());

            NIT_LOG_ERR("Error compiling the vertex shader: %s", info_log.data());

            // We don't need the shader anymore.
            glDeleteShader(vertex_shader);

            NIT_CHECK_MSG(false, "Vertex shader compilation failed!");
        }

        // Create an empty fragment shader handle
        const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that String's .c_str is NULL character terminated.
        source = fragment_source;
        glShaderSource(fragment_shader, 1, &source, nullptr);

        // Compile the fragment shader
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            GLint max_length = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

            // The maxLength includes the NULL character
            TArray<GLchar> info_log(max_length);
            glGetShaderInfoLog(fragment_shader, max_length, &max_length, info_log.data());
            NIT_LOG_ERR("Error compiling the fragment shader: %s", info_log.data());

            // We don't need the shader anymore.
            glDeleteShader(fragment_shader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertex_shader);

            NIT_CHECK_MSG(false, "Fragment shader compilation failed!");
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        id = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(id, vertex_shader);
        glAttachShader(id, fragment_shader);

        // Link our program
        glLinkProgram(id);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint is_linked = 0;
        glGetProgramiv(id, GL_LINK_STATUS, &is_linked);
        if (is_linked == GL_FALSE)
        {
            GLint max_length = 0;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);

            // The maxLength includes the NULL character
            TArray<GLchar> info_log(max_length);
            glGetProgramInfoLog(id, max_length, &max_length, info_log.data());
            NIT_LOG_ERR("Error linking the shaders: %s", info_log.data());

            // We don't need the program anymore.
            glDeleteProgram(id);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            NIT_CHECK_MSG(false, "Shader linkage failed!");
        }

        // Always detach shaders after a successful link.
        glDetachShader(id, vertex_shader);
        glDetachShader(id, fragment_shader);

        compiled = true;
#endif
    }

    void Shader::GetConstantCollection(TArray<TUniquePtr<Constant>>& constants) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        i32 uniform_count;
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniform_count);

        for (i32 i = 0; i < uniform_count; ++i)
        {
            constexpr i32 buf_size = 32;
            char name[buf_size];
            u32 type;
            i32 lenght;
            i32 size;
            glGetActiveUniform(id, i, buf_size, &lenght, &size, &type, name);
            auto constant = CreateUniquePtr<Constant>(name, ShaderDataTypeFromOpenGL(type), size);
            if (!constant) { continue; }
            constants.push_back(std::move(constant));
        }
#endif
    }

    void Shader::SetConstantFloat(const char* name, f32 value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniform1f(location, value);
#endif
    }

    void Shader::SetConstantVec2(const char* name, const f32* value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniform2fv(location, 1, value);
#endif
    }

    void Shader::SetConstantVec3(const char* name, const f32* value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniform3fv(location, 1, value);
#endif
    }

    void Shader::SetConstantMat4(const char* name, const f32* value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniformMatrix4fv(location, 1, false, value);
#endif
    }

    void Shader::SetConstantVec4(const char* name, const f32* value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniform4fv(location, 1, value);
#endif
    }

    void Shader::SetConstantInt(const char* name, i32 value) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const u32 location = glGetUniformLocation(id, name);
        glUniform1i(location, value);
#endif
    }

    void Shader::SetConstantSampler2D(const char* name, const i32* value, i32 size) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const i32 location = glGetUniformLocation(id, name);
        glUniform1iv(location, size, value);
#endif
    }

    void Shader::SetConstantObjects(const char* name, i32 binding_point, i32 max_objects, i32 size_object,
                                    const void* objects) const
    {
        // Create UBO
#ifdef NIT_GRAPHICS_API_OPENGL
        GLuint ubo_objects;
        glGenBuffers(1, &ubo_objects);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_objects);

        // Allocate memory for the UBO
        glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(max_objects) * size_object, objects, GL_DYNAMIC_DRAW);

        // Bind the UBO to binding point
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo_objects);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif
    }

    void Shader::Bind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glUseProgram(id);
#endif
    }

    void Shader::Unbind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glUseProgram(0);
#endif
    }
}