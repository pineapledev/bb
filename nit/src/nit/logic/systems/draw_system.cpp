#include "nit/render/renderer_2d.h"
#include "nit/render/render_api.h"
#include "nit/core/system.h"
#include "nit/core/app.h"
#include "nit/logic/entity.h"
#include "nit/logic/components/Transform.h"
#include "nit/logic/components/Camera.h"
#include "nit/logic/components/Sprite.h"

namespace Nit
{
    void Start();
    void Draw();
    
    void CreateDrawSystem()
    {
        CreateSystem("Draw");
        SetSystemCallback(Draw,  Stage::Draw);

        RegisterComponentType<Transform>();
        RegisterComponentType<Camera>();
        RegisterComponentType<Sprite>();
        
        CreateEntityGroup<Sprite, Transform>();
        CreateEntityGroup<Camera, Transform>();
    }

    void Draw()
    {
        ClearScreen();
        
        auto& camera_group = GetEntityGroup<Camera, Transform>();
        
        if (camera_group.entities.empty())
        {
            return;
        }

        TEntity main_camera = *camera_group.entities.begin();
        
        BeginScene2D(CalculateProjectionViewMatrix(GetComponent<Camera>(main_camera), GetComponent<Transform>(main_camera)));
        {
            for (TEntity entity : GetEntityGroup<Sprite, Transform>().entities)
            {
                auto& transform = GetComponent<Transform>(entity);
                auto& sprite = GetComponent<Sprite>(entity);

                if (sprite.texture)
                {
                    FillQuadVertexPositions(
                          sprite.vertex_data.vertex_positions
                        , sprite.texture, sprite.tiling_factor
                        , sprite.keep_aspect);
                
                    FillQuadVertexUVs(
                          sprite.vertex_data.vertex_uvs
                        , sprite.flip_x
                        , sprite.flip_y
                        , sprite.tiling_factor);
                    
                    TransformVertexPositions(sprite.vertex_data.vertex_positions, ToMatrix4(transform));
                }
                else
                {
                    sprite.vertex_data.vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
                    TransformVertexPositions(sprite.vertex_data.vertex_positions, ToMatrix4(transform));
                }
                
                FillVertexColors(sprite.vertex_data.vertex_colors, sprite.tint);
                DrawQuad(sprite.vertex_data, sprite.texture);
            }
        }
        EndScene2D();
    }

    void End()
    {
        FinishRenderer2D();
    }
}
