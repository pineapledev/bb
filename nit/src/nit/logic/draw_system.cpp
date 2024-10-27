#include "nit/render/primitives_2d.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/render_api.h"
#include "nit/core/system.h"
#include "nit/core/app.h"
#include "nit/logic/entity.h"
#include "nit/logic/components.h"
#include "nit/render/texture.h"
#include "nit/render/font.h"

namespace Nit
{
    V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
    V2Verts2D vertex_uvs       = DEFAULT_VERTEX_U_VS_2D;
    V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;
    
    void Start();
    void End();
    void Draw();
    
    ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args);
    ListenerAction OnComponentAdded(const ComponentAddedArgs& args);
    ListenerAction OnComponentRemoved(const ComponentRemovedArgs& args);
    
    void CreateDrawSystem()
    {
        CreateSystem("Draw");
        SetSystemCallback(Start, Stage::Start);
        SetSystemCallback(End,   Stage::End);
        SetSystemCallback(Draw,  Stage::Draw);
        
        CreateEntityGroup<Sprite, Transform>();
        CreateEntityGroup<Camera, Transform>();
        CreateEntityGroup<Circle, Transform>();
        CreateEntityGroup<Line2D, Transform>();
        CreateEntityGroup<Text,   Transform>();
    }
    
    void Start()
    {
        app->asset_registry.asset_destroyed_event    += Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
        app->entity_registry.component_added_event   += Listener<const ComponentAddedArgs&>::Create(OnComponentAdded);
        app->entity_registry.component_removed_event += Listener<const ComponentRemovedArgs&>::Create(OnComponentRemoved);
    }

    void End()
    {
        app->asset_registry.asset_destroyed_event    -= Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
        app->entity_registry.component_added_event   -= Listener<const ComponentAddedArgs&>::Create(OnComponentAdded);
        app->entity_registry.component_removed_event -= Listener<const ComponentRemovedArgs&>::Create(OnComponentRemoved);
    }
    
    ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args)
    {
        if (args.asset_handle.type == GetType<Texture2D>())
        {
            for (Entity entity : GetEntityGroup<Sprite, Transform>().entities)
            {
                auto& sprite = GetComponent<Sprite>(entity);
            
                if (!IsAssetValid(sprite.texture))
                {
                    sprite.texture.id = 0;
                    sprite.texture_data = nullptr;
                    continue;
                }

                sprite.texture_data = GetAssetDataPtr<Texture2D>(sprite.texture);
            }
        }
        else if (args.asset_handle.type == GetType<Font>())
        {
            for (Entity entity : GetEntityGroup<Text, Transform>().entities)
            {
                auto& text = GetComponent<Text>(entity);
            
                if (!IsAssetValid(text.font))
                {
                    text.font.id = 0;
                    text.font_data = nullptr;
                    continue;
                }

                text.font_data = GetAssetDataPtr<Font>(text.font);
            }
        }
        
        return ListenerAction::StayListening;
    }

    ListenerAction OnComponentAdded(const ComponentAddedArgs& args)
    {
        if (args.type == GetType<Sprite>())
        {
            Sprite& sprite = GetComponent<Sprite>(args.entity);
            AddTextureToSprite(sprite, sprite.texture);
        }
        else if (args.type == GetType<Text>())
        {
            Text& text = GetComponent<Text>(args.entity);
            AddFontToText(text, text.font);
        }
        return ListenerAction::StayListening;
    }

    ListenerAction OnComponentRemoved(const ComponentRemovedArgs& args)
    {
        if (args.type == GetType<Sprite>())
        {
            Sprite& sprite = GetComponent<Sprite>(args.entity);
            RemoveTextureFromSprite(sprite);
        }
        else if (args.type == GetType<Text>())
        {
            Text& text = GetComponent<Text>(args.entity);
            RemoveFontFromText(text);
        }
        return ListenerAction::StayListening;
    }
    
    void Draw()
    {
        ClearScreen();
        
        auto& camera_group = GetEntityGroup<Camera, Transform>();
        
        if (camera_group.entities.empty())
        {
            return;
        }
        
        Entity main_camera = *camera_group.entities.begin();
        Camera& camera = GetComponent<Camera>(main_camera);

        i32 width, height;
        RetrieveWindowSize(&width, &height);
        
#ifdef NIT_EDITOR_ENABLED
        if (app->editor.enabled && app->editor.show_viewport)
        {
            ClearAttachment(&app->editor.frame_buffer, 1, -1);
            width  = app->editor.frame_buffer.width;
            height = app->editor.frame_buffer.height;
        }
#endif

        SetViewport(0, 0, width, height);
        
        camera.aspect = (f32) width / (f32) height;   
        
        if (isnan(camera.aspect))
        {
            return;
        }

        SetDepthTestEnabled(camera.projection == CameraProjection::Perspective);
        
        BeginScene2D(CalculateProjectionViewMatrix(camera, GetComponent<Transform>(main_camera)));
        {
            for (Entity entity : GetEntityGroup<Sprite, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& sprite = GetComponent<Sprite>(entity);

                if (!sprite.visible || sprite.tint.w <= F32_EPSILON)
                {
                    continue;
                }
                
                if (sprite.texture_data)
                {
                    Vector2 size = sprite.texture_data->size;

                    if (sprite.texture_data->sub_textures
                        && sprite.sub_texture_index >= 0
                        && (u32) sprite.sub_texture_index < sprite.texture_data->sub_texture_count)
                    {
                        const SubTexture2D& sub_texture = sprite.texture_data->sub_textures[sprite.sub_texture_index];
                        size = sub_texture.size;
                        
                        FillQuadVertexUVs(
                              vertex_uvs
                            , sprite.texture_data->size
                            , sub_texture.size
                            , sub_texture.location
                            , sprite.flip_x
                            , sprite.flip_y
                            , sprite.tiling_factor);
                    }
                    else
                    {
                        FillQuadVertexUVs(
                          vertex_uvs
                        , sprite.flip_x
                        , sprite.flip_y
                        , sprite.tiling_factor);
                    }

                    if (sprite.keep_aspect)
                    {
                        FillQuadVertexPositions(size , vertex_positions);
                    }
                    else
                    {
                        vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
                    }
                    
                    TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                }
                else
                {
                    vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
                    TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                }
                
                FillVertexColors(vertex_colors, sprite.tint);
                DrawQuad(sprite.texture_data, vertex_positions, vertex_uvs, vertex_colors, (i32) entity);
            }

            for (Entity entity : GetEntityGroup<Line2D, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& line = GetComponent<Line2D>(entity);

                if (!line.visible || line.tint.w <= F32_EPSILON )
                {
                    continue;
                }
                
                FillLine2DVertexPositions(vertex_positions, line.start, line.end, line.thickness);
                TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                FillVertexColors(vertex_colors, line.tint);
                DrawLine2D(vertex_positions, vertex_colors, (i32) entity);
            }

            for (Entity entity : GetEntityGroup<Text, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& text = GetComponent<Text>(entity);

                if (!text.visible || text.text.empty() || !text.font_data)
                {
                    continue;
                }
                
                DrawText(
                      text.font_data
                    , text.text
                    , ToMatrix4(transform)
                    , text.tint
                    , text.spacing
                    , text.size
                    , (i32) entity
                );
            }

            for (Entity entity : GetEntityGroup<Circle, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& circle = GetComponent<Circle>(entity);

                if (!circle.visible || circle.tint.w <= F32_EPSILON)
                {
                    continue;
                }
                
                FillCircleVertexPositions(vertex_positions, circle.radius);
                TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                FillVertexColors(vertex_colors, circle.tint);
                DrawCircle(vertex_positions, vertex_colors, circle.thickness, circle.fade, (i32) entity);
            }
        }
        EndScene2D();
    }
}