#include "nit/render/primitives_2d.h"
#include "logic/components/line_2d.h"
#include "logic/components/text.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/render_api.h"
#include "nit/core/system.h"
#include "nit/core/app.h"
#include "nit/logic/entity.h"
#include "nit/logic/components/transform.h"
#include "nit/logic/components/camera.h"
#include "nit/logic/components/sprite.h"
#include "nit/logic/components/circle.h"

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
        app->asset_registry.asset_destroyed_event  += Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
        app->entity_registry.component_added_event += Listener<const ComponentAddedArgs&>::Create(OnComponentAdded);
    }

    void End()
    {
        app->asset_registry.asset_destroyed_event    -= Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
        app->entity_registry.component_removed_event -= Listener<const ComponentRemovedArgs&>::Create(OnComponentRemoved);
    }
    
    ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args)
    {
        if (args.type == GetType<Texture2D>())
        {
            for (Entity entity : GetEntityGroup<Sprite, Transform>().entities)
            {
                auto& sprite = GetComponent<Sprite>(entity);
            
                if (!IsAssetValid(sprite.texture_id))
                {
                    sprite.texture_id = 0;
                    sprite.texture = nullptr;
                    continue;
                }

                sprite.texture = GetAssetDataPtr<Texture2D>(sprite.texture_id);
            }
        }
        else if (args.type == GetType<Font>())
        {
            for (Entity entity : GetEntityGroup<Text, Transform>().entities)
            {
                auto& text = GetComponent<Text>(entity);
            
                if (!IsAssetValid(text.font_id))
                {
                    text.font_id = 0;
                    text.font = nullptr;
                    continue;
                }

                text.font = GetAssetDataPtr<Font>(text.font_id);
            }
        }
        
        return ListenerAction::StayListening;
    }

    ListenerAction OnComponentAdded(const ComponentAddedArgs& args)
    {
        if (args.type == GetType<Sprite>())
        {
            Sprite& sprite = GetComponent<Sprite>(args.entity);
            AddTextureToSprite(sprite, sprite.texture_id);
        }
        else if (args.type == GetType<Text>())
        {
            Text& text = GetComponent<Text>(args.entity);
            AddFontToText(text, text.font_id);
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
        
        BeginScene2D(CalculateProjectionViewMatrix(GetComponent<Camera>(main_camera), GetComponent<Transform>(main_camera)));
        {
            for (Entity entity : GetEntityGroup<Sprite, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& sprite = GetComponent<Sprite>(entity);

                if (!sprite.visible || sprite.tint.w <= F32_EPSILON)
                {
                    continue;
                }
                
                if (sprite.texture)
                {
                    FillQuadVertexPositions(
                          vertex_positions
                        , sprite.texture, sprite.tiling_factor
                        , sprite.keep_aspect);
                
                    FillQuadVertexUVs(
                          vertex_uvs
                        , sprite.flip_x
                        , sprite.flip_y
                        , sprite.tiling_factor);
                    
                    TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                }
                else
                {
                    vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
                    TransformVertexPositions(vertex_positions, ToMatrix4(transform));
                }
                
                FillVertexColors(vertex_colors, sprite.tint);
                DrawQuad(sprite.texture, vertex_positions, vertex_uvs, vertex_colors);
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
                DrawCircle(vertex_positions, vertex_colors, circle.thickness, circle.fade);
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
                DrawLine2D(vertex_positions, vertex_colors);
            }

            for (Entity entity : GetEntityGroup<Text, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& text = GetComponent<Text>(entity);

                if (!text.visible || text.text.empty() || !text.font)
                {
                    continue;
                }
                
                DrawText(
                      text.font
                    , text.text
                    , ToMatrix4(transform)
                    , text.tint
                    , text.spacing
                    , text.size
                );
            }
        }
        EndScene2D();
    }
}