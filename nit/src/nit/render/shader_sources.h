#pragma once

namespace Nit
{
    inline auto quad_flat_color_vertex_shader_source = R"(
            #version 420 core
            
            layout(location = 0) in vec4 a_Position;
            layout(location = 1) in vec4 a_Tint;
            layout(location = 2) in int  a_EntityID;

            uniform mat4 u_ProjectionView;        
            
            out vec4     v_Tint;
            flat out int v_EntityID;

            void main()
            {
                gl_Position   = u_ProjectionView * a_Position;
                v_Tint        = a_Tint;
                v_EntityID    = a_EntityID;
            }
        )";

    inline auto quad_flat_color_fragment_shader_source = R"(
            #version 420 core
            
            layout(location = 0) out vec4 o_Color;
            layout(location = 1) out int  o_EntityID;
            
            in vec4      v_Tint;
            flat in int  v_EntityID;

            void main()
            {
                o_Color    = v_Tint;
                o_EntityID = v_EntityID;
            }
        )";

    inline auto quad_texture_vertex_shader_source = R"(
            #version 420 core
            
            layout(location = 0) in vec4 a_Position;
            layout(location = 1) in vec4 a_Tint;
            layout(location = 2) in vec2 a_UV;
            layout(location = 3) in int  a_Texture;
            layout(location = 4) in int  a_EntityID;

            uniform mat4 u_ProjectionView;        
            
            out vec4     v_Tint;
            out vec2     v_UV;
            flat out int v_Texture;
            flat out int v_EntityID;

            void main()
            {
                gl_Position   = u_ProjectionView * a_Position;
                v_Tint        = a_Tint;
                v_UV          = a_UV;
                v_Texture     = a_Texture;
                v_EntityID    = a_EntityID;
            }
        )";

    inline auto quad_texture_fragment_shader_source = R"(
            #version 420 core
            
            layout(location = 0) out vec4 o_Color;
            layout(location = 1) out int  o_EntityID;
            
            uniform sampler2D u_Textures[32];
            
            in vec4      v_Tint;
            in vec2      v_UV;
            flat in int  v_Texture;
            flat in int  v_EntityID;

            void main()
            {
                o_Color    = texture(u_Textures[v_Texture], v_UV) * v_Tint;
                o_EntityID = v_EntityID;
            }
        )";

    inline auto circle_vertex_shader_source = R"(
            #version 420 core

            layout(location = 0) in vec4  a_Position;
            layout(location = 1) in vec4  a_LocalPosition;
            layout(location = 2) in vec4  a_Tint;
            layout(location = 3) in float a_Thickness;
            layout(location = 4) in float a_Fade;
            layout(location = 5) in int   a_EntityID;

            uniform mat4 u_ProjectionView;

            // Vertex output
            out vec4     v_LocalPosition;
            out vec4     v_Tint;
            out float    v_Thickness;
            out float    v_Fade;
            flat out int v_EntityID;

            void main()
            {
                gl_Position     = u_ProjectionView * a_Position;
        
                v_LocalPosition = a_LocalPosition;
                v_Tint          = a_Tint;
                v_Thickness     = a_Thickness;
                v_Fade          = a_Fade;
                v_EntityID      = a_EntityID;
            }
        )";

    inline auto circle_fragment_shader_source = R"(
            #version 420 core

            layout(location = 0) out vec4 o_Color;
            layout(location = 1) out int  o_EntityID;

            // Vertex input
            in vec4     v_LocalPosition;
            in vec4     v_Tint;
            in float    v_Thickness;
            in float    v_Fade;
            flat in int v_EntityID;

            void main()
            {
                vec2 localPos = vec2(v_LocalPosition.x * 2, v_LocalPosition.y * 2);
                float d = 1.0 - length(localPos);
                float alpha = smoothstep(0.0, v_Fade, d);

                alpha *= smoothstep(v_Thickness + v_Fade, v_Thickness, d);

                o_Color = vec4(v_Tint.rgb, alpha);
                o_EntityID = v_EntityID;    
            }
        )";

    inline auto text_vertex_shader_source = R"(
            #version 420 core
            
            layout(location = 0) in vec4 a_Position;
            layout(location = 1) in vec4 a_Tint;
            layout(location = 2) in vec2 a_UV;
            layout(location = 3) in int  a_Texture;
            layout(location = 4) in int  a_EntityID;

            uniform mat4 u_ProjectionView;        
            
            out vec4     v_Tint;
            out vec2     v_UV;
            flat out int v_Texture;
            flat out int v_EntityID;

            void main()
            {
                gl_Position   = u_ProjectionView * a_Position;
                v_Tint        = a_Tint;
                v_UV          = a_UV;
                v_Texture     = a_Texture;
                v_EntityID    = a_EntityID;
            }
        )";

    inline auto text_fragment_shader_source = R"(
            #version 420 core
            
            layout(location = 0) out vec4 o_Color;
            layout(location = 1) out int  o_EntityID;
            
            uniform sampler2D u_Textures[32];
            
            in vec4      v_Tint;
            in vec2      v_UV;
            flat in int  v_Texture;
            flat in int  v_EntityID;

            void main()
            {
                o_Color    = texture(u_Textures[v_Texture], v_UV) * v_Tint;
                o_EntityID = v_EntityID;
            }
        )";

    inline auto lit_vertex_shader_source = R"(
            #version 420 core
            
            layout(location = 0) in vec3 a_LocalPosition;
            layout(location = 1) in vec3 a_Normal;

            uniform mat4 u_ProjectionView;
            uniform mat4 u_Model;       
            
            out vec3 v_FragPosition;
            out vec3 v_Normal;

            float MapRange(float inValue, float minInRange, float maxInRange, float minOutRange, float maxOutRange)
            {
                float x = (inValue - minInRange) / (maxInRange - minInRange);
                return minOutRange + (maxOutRange - minOutRange) * x;
            }

            void main()
            {
                gl_Position    = u_ProjectionView * u_Model * vec4(a_LocalPosition, 1);
                v_FragPosition = vec3(u_Model * vec4(a_LocalPosition, 1));
                v_Normal       = mat3(transpose(inverse(u_Model))) * a_Normal;
            }
        )";

    inline auto lit_fragment_shader_source = R"(
            #version 420 core
            
            layout(location = 0) out vec4 o_Color;
            layout(location = 1) out int  o_EntityID;
          
            in vec3 v_FragPosition;
            in vec3 v_Normal;

            struct MaterialData {
                float   ambient;
                float   diffuse;
                float   specular;
                float   shininess;
                vec3    color;
            };
            layout(std140, binding = 0) uniform MaterialBlock {
                MaterialData u_Material;
            };

            struct LightSource {
                vec3 position;
                vec3 viewPosition;
                vec3 color;
                vec3 direction;
                float specularStrength;
                float diffuseStrength;
                float ambientStrength;
                float constantAtt;
                float linearAtt;
                float quadAtt;
                float angle;
                float outerAngle;
                int type;
            };

            uniform int i_NumLights;
            layout (std140, binding = 1) uniform LightsBlock {
                LightSource u_Lights [10];
            };

            
            void main()
            {
                vec3 diffuseTotal   = vec3(0);
                vec3 specularTotal  = vec3(0);

                // Ambient
                vec3 ambient   = u_Lights[0].color * u_Lights[0].ambientStrength * vec3(u_Material.color) * u_Material.ambient;

                for (int i = 1; i < i_NumLights; i++)
                {
                    float f_CutOff = cos(u_Lights[i].angle * (3.14 / 180));
                    float f_OuterCuttoff = cos(u_Lights[i].outerAngle * (3.14 / 180));

                    float f_Attenuation = 1;
                    vec3 v_LightDir     = vec3(0.0);

                    if (u_Lights[i].type == 0)
                    {
                        v_LightDir = normalize(-u_Lights[i].direction);
                    }
                    else
                    {
                        v_LightDir      = normalize(u_Lights[i].position - v_FragPosition);
                        float distance  = length(u_Lights[i].viewPosition - v_FragPosition);
                        if (u_Lights[i].constantAtt + u_Lights[i].linearAtt + u_Lights[i].quadAtt != 0)
                        {
                            f_Attenuation   = 1 / (u_Lights[i].constantAtt + u_Lights[i].linearAtt * distance + u_Lights[i].quadAtt * distance * distance);
                        }
                    }

                    
                    vec3 diffuse   = vec3(0.0);
                    vec3 specular  = vec3(0.0);

                    float f_DotProduct  = 0;
                    float f_Epsilon     = 0;
                    float f_Intensity   = 1;

                    if (u_Lights[i].type == 2)
                    {
                        f_DotProduct = dot(v_LightDir, normalize(-u_Lights[i].direction));
                        f_Epsilon   = f_CutOff - f_OuterCuttoff;
                        f_Intensity = clamp((f_DotProduct - f_OuterCuttoff) / f_Epsilon, 0.0, 1.0);   
                    }

                    if (u_Lights[i].type != 2 || (u_Lights[i].type == 2 && f_DotProduct > f_OuterCuttoff))
                    {
                        // Diffuse
                        vec3  norm      = normalize(v_Normal);
                        float diff      = max(dot(norm, v_LightDir), 0.0);
                        diffuse         = diff * u_Lights[i].color * u_Lights[i].diffuseStrength * vec3(u_Material.color) * u_Material.diffuse * f_Attenuation * f_Intensity;
                    
                        // Specular
                        vec3 viewDir    = normalize(u_Lights[i].viewPosition - v_FragPosition);
                        vec3 reflectDir = reflect(-v_LightDir, v_Normal);  
                        float spec      = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
                        specular        = u_Lights[i].color * u_Lights[i].specularStrength * spec * vec3(u_Material.color) * u_Material.specular * f_Attenuation * f_Intensity;

                    }

                    diffuseTotal    = diffuseTotal + diffuse;
                    specularTotal   = specularTotal + specular;
                }
                o_Color    = vec4(ambient + diffuseTotal + specularTotal, 1.f);
            }
        )";
}
