#include "Camera.h"

namespace Nit
{
    Matrix4 CalculateProjectionViewMatrix(const Camera& camera, const Transform& transform)
    {
        Matrix4 proj;

        switch (camera.projection)
        {
        case CameraProjection::Perspective:
            {
                proj = PerspectiveProjection(camera.fov, camera.aspect, camera.near_clip, camera.far_clip);
            }
            break;
        case CameraProjection::Orthographic:
            {
                const f32 right = camera.aspect * camera.size;
                const f32 left = -right;
                proj = OrthographicProjection(left, right, -camera.size, camera.size, camera.near_clip, camera.far_clip);
            }
            break;
        default:
            NIT_CHECK(false);
            break;
        }

        return proj * Inverse(ToMatrix4(transform));
    }
}