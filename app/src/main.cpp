#include "nit.h"

int main(int argc, char** argv)
{
    using namespace nit;
    
    TypeRegistry   type_registry;
    Window         window;
    RenderObjects  render_objects;
    Renderer2D     renderer_2d;
    
    type_registry_set_instance(&type_registry);
    type_registry_init();
    
    window_set_instance(&window);
    window_init({ .start_maximized = false });
    
    render_objects_set_instance(&render_objects);
    render_objects_init();
    
    renderer_2d_set_instance(&renderer_2d);
    renderer_2d_init();
    
    while (!window_should_close())
    {
        clear_screen();
        
        begin_scene_2d(Matrix4{});
        draw_quad({.2, 0,  0});
        end_scene_2d();
        
        window_update();
    }
}