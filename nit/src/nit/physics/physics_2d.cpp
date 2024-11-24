#include "physics_2d.h"

#include "rigidbody_2d.h"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "core/engine.h"

namespace nit
{
    Physics2D* physics_2d = nullptr;
    
    void physics_2d_set_instance(Physics2D* instance)
    {
        if (!instance || physics_2d)
        {
            NIT_CHECK(false);
            return;
        }

        physics_2d = instance;
    }

    bool physics_2d_has_instance()
    {
        return physics_2d != nullptr;
    }

    Physics2D* physics_2d_get_instance()
    {
        if (!physics_2d)
        {
            NIT_CHECK(false);
            return nullptr;
        }

        return physics_2d;
    }

    // Pre-declared listeners
    static ListenerAction start();
    static ListenerAction update();
    static ListenerAction fixed_update();
    static ListenerAction draw();
    
    void physics_2d_init()
    {
        if (!physics_2d_has_instance())
        {
            static Physics2D instance;
            physics_2d_set_instance(&instance);
        }
        
        engine_event(Stage::Start)       += EngineListener::create(start);
        engine_event(Stage::Update)      += EngineListener::create(update);
        engine_event(Stage::FixedUpdate) += EngineListener::create(fixed_update);
        engine_event(Stage::Draw)        += EngineListener::create(draw);
    }

    void physics_2d_finish()
    {
        if (!physics_2d_has_instance())
        {
            NIT_CHECK(false);
            return;
        }

        if (physics_2d->world)
        {
            delete (b2World*) physics_2d->world;
            physics_2d->world = nullptr;
        }
    }

    static b2World* world()
    {
        if (physics_2d_has_instance())
        {
            NIT_CHECK(false);
            return nullptr;
        }

        if (!physics_2d->world)
        {
            physics_2d->world = new b2World((b2Vec2&) physics_2d->gravity);
        }

        return (b2World*) physics_2d;
    }

    static void rigidbody_init(Rigidbody2D& rb, const Vector2& position, f32 angle)
    {
        b2BodyDef def;
        def.type     = (b2BodyType) rb.body_type;
        def.position = (const b2Vec2&) position;
        def.angle    = angle;
        
        rb.prev_body_type = rb.body_type; 
        rb.body_ptr       = world()->CreateBody(&def);
    }

    ListenerAction start()        { return ListenerAction::StayListening; }
    ListenerAction update()       { return ListenerAction::StayListening; }
    ListenerAction fixed_update() { return ListenerAction::StayListening; }
    ListenerAction draw()         { return ListenerAction::StayListening; }
}