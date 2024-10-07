#include "logic/components/line_2d.h"
#include "logic/components/text.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/primitives_2d.h"
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
    
    //TODO: ON COMPONENT REMOVED RELEASE THE SPRITE TEXTURE
    
    void Start()
    {
        app->asset_registry.asset_destroyed_event += Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
    }

    void End()
    {
        app->asset_registry.asset_destroyed_event -= Listener<const AssetDestroyedArgs&>::Create(OnAssetDestroyed);
    }
    
    ListenerAction OnAssetDestroyed(const AssetDestroyedArgs& args)
    {
        if (args.type != GetType<Texture2D>())
        {
            return ListenerAction::StayListening;    
        }
        
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

                if (!text.visible || text.text.empty())
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