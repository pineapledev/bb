#include "nit/render/primitives_2d.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/render_api.h"
#include "nit/core/system.h"
#include "nit/core/engine.h"
#include "nit/logic/entity.h"
#include "nit/logic/components.h"
#include "nit/render/texture.h"
#include "nit/render/font.h"

namespace nit::FnDrawSystem
{
    V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
    V2Verts2D vertex_uvs       = DEFAULT_VERTEX_U_VS_2D;
    V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;
    
    void Start();
    void End();
    void Draw();
    
    static ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args);
    static ListenerAction OnComponentAdded(const ComponentAddedArgs& args);
    static ListenerAction OnComponentRemoved(const ComponentRemovedArgs& args);

    Entity GetMainCamera()
    {
        auto& camera_group = GetEntityGroup<Camera, Transform>();
        
        if (camera_group.entities.empty())
        {
            return NULL_ENTITY;
        }

#ifdef NIT_EDITOR_ENABLED

        if (!engine::GetInstance()->editor.enabled)
        {
            for (Entity entity : camera_group.entities)
            {
                if (!HasComponent<EditorCameraController>(entity))
                {
                    return entity;
                }
            }
        }

        return engine::GetInstance()->editor.editor_camera_entity;
        
#else
        return *camera_group.entities.begin();
#endif
    }
    
    void Register()
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
        engine::GetInstance()->asset_registry.asset_destroyed_event    += AssetDestroyedListener::Create(OnAssetDestroyed);
        engine::GetInstance()->entity_registry.component_added_event   += ComponentAddedListener::Create(OnComponentAdded);
        engine::GetInstance()->entity_registry.component_removed_event += ComponentRemovedListener::Create(OnComponentRemoved);
    }

    void End()
    {
        engine::GetInstance()->asset_registry.asset_destroyed_event    -= AssetDestroyedListener::Create(OnAssetDestroyed);
        engine::GetInstance()->entity_registry.component_added_event   -= ComponentAddedListener::Create(OnComponentAdded);
        engine::GetInstance()->entity_registry.component_removed_event -= ComponentRemovedListener::Create(OnComponentRemoved);
    }
    
    static ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args)
    {
        if (args.asset_handle.type == GetType<Texture2D>())
        {
            
        }
        else if (args.asset_handle.type == GetType<Font>())
        {
            
        }
        
        return ListenerAction::StayListening;
    }

    static ListenerAction OnComponentAdded(const ComponentAddedArgs& args)
    {
        if (args.type == GetType<Sprite>())
        {
            auto& sprite = GetComponent<Sprite>(args.entity); 
            auto& asset = sprite.texture;

            RetargetAssetHandle(asset);

            bool is_valid = IsAssetValid(asset);
            
            if (is_valid && !IsAssetLoaded(asset))
            {
                RetainAsset(asset);
            }

            if (is_valid)
            {
                sprite.sub_texture_index = FindIndexOfSubTexture2D(GetAssetData<Texture2D>(asset), sprite.sub_texture);
            }
            else
            {
                sprite.sub_texture_index = -1;
            }
        }
        else if (args.type == GetType<Text>())
        {
            auto& asset = GetComponent<Text>(args.entity).font;
            RetargetAssetHandle(asset);
            if (IsAssetValid(asset) && !IsAssetLoaded(asset))
            {
                RetainAsset(asset);
            }
        }
        return ListenerAction::StayListening;
    }

    static ListenerAction OnComponentRemoved(const ComponentRemovedArgs& args)
    {
        if (args.type == GetType<Sprite>())
        {
            auto& sprite = GetComponent<Sprite>(args.entity); 
            auto& asset = sprite.texture;
            RetargetAssetHandle(asset);
            
            if (IsAssetValid(asset) && IsAssetLoaded(asset))
            {
                ReleaseAsset(asset);
            }
        }
        else if (args.type == GetType<Text>())
        {
            auto& asset = GetComponent<Text>(args.entity).font;
            RetargetAssetHandle(asset);
            if (IsAssetValid(asset) && IsAssetLoaded(asset))
            {
                ReleaseAsset(asset);
            }
        }
        return ListenerAction::StayListening;
    }
    
    void Draw()
    {
        ClearScreen();
        
        Entity main_camera = GetMainCamera();

        Camera& camera = GetComponent<Camera>(main_camera);

        i32 width, height;
        window::RetrieveSize(&width, &height);
        
#ifdef NIT_EDITOR_ENABLED
        if (engine::GetInstance()->editor.enabled && engine::GetInstance()->editor.show_viewport)
        {
            ClearAttachment(&engine::GetInstance()->editor.frame_buffer, 1, -1);
            width  = engine::GetInstance()->editor.frame_buffer.width;
            height = engine::GetInstance()->editor.frame_buffer.height;
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

                bool has_texture = IsAssetValid(sprite.texture); 

                Texture2D* texture_data = has_texture ? GetAssetData<Texture2D>(sprite.texture) : nullptr; 
                
                if (has_texture)
                {
                    if (!IsAssetLoaded(sprite.texture))
                    {
                        RetainAsset(sprite.texture);
                    }
                    
                    Vector2 size = texture_data->size;

                    if (texture_data->sub_textures
                        && sprite.sub_texture_index >= 0
                        && (u32) sprite.sub_texture_index < texture_data->sub_texture_count)
                    {
                        const SubTexture2D& sub_texture = texture_data->sub_textures[sprite.sub_texture_index];
                        size = sub_texture.size;
                        
                        FillQuadVertexUVs(
                              vertex_uvs
                            , texture_data->size
                            , sub_texture.size
                            , sub_texture.location
                            , sprite.flip_x
                            , sprite.flip_y
                            , sprite.tiling_factor);
                    }
                    else
                    {
                        vertex_uvs = DEFAULT_VERTEX_U_VS_2D;
                        
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
                DrawQuad(texture_data, vertex_positions, vertex_uvs, vertex_colors, (i32) entity);
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
                Font* font_data = IsAssetValid(text.font) ? GetAssetData<Font>(text.font) : nullptr;

                if (font_data && !IsAssetLoaded(text.font))
                {
                    RetainAsset(text.font);
                }
                
                if (!text.visible || text.text.empty() || !font_data)
                {
                    continue;
                }
                
                DrawText(
                      font_data
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