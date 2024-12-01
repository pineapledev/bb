#include "physics_2d.h"

#include "box_collider_2d.h"
#include "circle_collider.h"
#include "physic_material.h"
#include "rigidbody_2d.h"
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "core/engine.h"
#include "entity/entity_utils.h"
#include "render/transform.h"

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
    static ListenerAction fixed_update();
    static ListenerAction end();
    static ListenerAction on_component_added(const ComponentAddedArgs& args);
    static ListenerAction on_component_removed(const ComponentRemovedArgs& args);
    
    
    void physics_2d_init() 
    {
        if (!physics_2d_has_instance())
        {
            static Physics2D instance;
            physics_2d_set_instance(&instance);
        }

        entity_create_group<Transform, Rigidbody2D, BoxCollider2D>();
        entity_create_group<Transform, Rigidbody2D, CircleCollider>();
        
        engine_event(Stage::Start)       += EngineListener::create(start);
        engine_event(Stage::FixedUpdate) += EngineListener::create(fixed_update);
        engine_event(Stage::End)         += EngineListener::create(end);
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
        if (!physics_2d_has_instance())
        {
            NIT_CHECK(false);
            return nullptr;
        }

        if (!physics_2d->world)
        {
            physics_2d->world = new b2World((b2Vec2&) physics_2d->gravity);
        }

        return (b2World*) physics_2d->world;
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

    static void fixture_def_init(b2FixtureDef& def, const b2Shape& shape, AssetHandle& material_handle)
    {
        PhysicMaterial* physic_material;
        
        if (asset_valid(material_handle) && material_handle.type != type_get<PhysicMaterial>())
        {
            physic_material = asset_get_data<PhysicMaterial>(material_handle);
        }
        else
        {
            static PhysicMaterial default_physic_material;
            physic_material = &default_physic_material;
        }

        def.shape                = &shape;
        def.density              = physic_material->density;
        def.friction             = physic_material->friction;
        def.restitution          = physic_material->bounciness;
        def.restitutionThreshold = physic_material->threshold;
    }

    void box_collider_init(Rigidbody2D& rb, BoxCollider2D& collider)
    {
        if (!rb.body_ptr)
        {
            NIT_CHECK(false);
            return;
        }
        
        b2Body* body = (b2Body*) rb.body_ptr;
        
        if (collider.fixture_ptr)
        {
            body->DestroyFixture((b2Fixture*) collider.fixture_ptr);
        }

        b2PolygonShape shape;
        shape.SetAsBox(collider.size.x / 2.f, collider.size.y / 2.f);
        b2FixtureDef fixture_def;
        fixture_def_init(fixture_def, shape, collider.physic_material);
        collider.fixture_ptr = body->CreateFixture(&fixture_def);
        collider.prev_size = collider.size;
    }

    void circle_collider_init(Rigidbody2D& rb, CircleCollider& collider)
    {
        if (!rb.body_ptr)
        {
            NIT_CHECK(false);
            return;
        }
        
        b2Body* body = (b2Body*) rb.body_ptr;
        
        if (collider.fixture_ptr)
        {
            body->DestroyFixture((b2Fixture*) collider.fixture_ptr);
        }

        b2CircleShape shape;
        shape.m_radius = collider.radius;
        b2FixtureDef fixture_def;
        fixture_def_init(fixture_def, shape, collider.physic_material);
        collider.fixture_ptr = body->CreateFixture(&fixture_def);
        collider.prev_radius = collider.radius;
    }

    static void rigidbody_update(Rigidbody2D& rb, Transform& transform, const Vector2& center)
    {
        if (!rb.body_ptr)
        {
            NIT_CHECK(false);
            return;
        }

        b2Body* body = (b2Body*) rb.body_ptr;
        
        body->SetEnabled(rb.enabled);
        
        if (!rb.enabled)
        {
            return;
        }

        if (rb.body_type != rb.prev_body_type)
        {
            body->SetType( (b2BodyType) rb.body_type);
            
            if (rb.body_type == BodyType::Kinematic)
            {
                body->SetLinearVelocity((const b2Vec2&) V2_ZERO);
            }
        }

        body->SetAwake(!rb.follow_transform);
        
        if (rb.follow_transform)
        {
            body->SetTransform((const b2Vec2&) transform.position, to_radians(transform.rotation.z));
        }
        else
        {
            Vector2 body_pos     = (const Vector2&) body->GetTransform().p - center;
            transform.position = { body_pos.x, body_pos.y, transform.position.z };
            transform.rotation.z = to_degrees(body->GetAngle());
        }

        body->SetGravityScale(rb.gravity_scale);
    }

    static void update_bodies()
    {
        for (EntityID entity : entity_get_group<Transform, Rigidbody2D, BoxCollider2D>().entities)
        {
            auto& transform  = entity_get<Transform>(entity);
            auto& rb= entity_get<Rigidbody2D>(entity);
            auto& collider   = entity_get<BoxCollider2D>(entity);
            
            if (!rb.body_ptr)
            {
                rigidbody_init(rb, (const Vector2&) transform.position + collider.center, transform.rotation.z);
            }

            if (!collider.fixture_ptr || collider.size != collider.prev_size)
            {
                box_collider_init(rb, collider);
            }

            rigidbody_update(rb, transform, collider.center);
        }

        for (EntityID entity : entity_get_group<Transform, Rigidbody2D, CircleCollider>().entities)
        {
            auto& transform  = entity_get<Transform>(entity);
            auto& rb= entity_get<Rigidbody2D>(entity);
            auto& collider   = entity_get<CircleCollider>(entity);
            
            if (!rb.body_ptr)
            {
                rigidbody_init(rb, (const Vector2&) transform.position + collider.center, transform.rotation.z);
            }
        
            if (!collider.fixture_ptr || !epsilon_equal(collider.radius, collider.prev_radius))
            {
                circle_collider_init(rb, collider);
            }
        
            rigidbody_update(rb, transform, collider.center);
        }
    }
    
    ListenerAction start()
    {
        engine_get_instance()->entity_registry.component_added_event   += ComponentAddedListener::create(on_component_added);
        engine_get_instance()->entity_registry.component_removed_event += ComponentRemovedListener::create(on_component_removed);
        
        return ListenerAction::StayListening;
    }

    ListenerAction end()
    {
        engine_get_instance()->entity_registry.component_added_event   -= ComponentAddedListener::create(on_component_added);
        engine_get_instance()->entity_registry.component_removed_event -= ComponentRemovedListener::create(on_component_removed);
        return ListenerAction::StayListening;
    }

    ListenerAction on_component_added(const ComponentAddedArgs& args)
    {
        if (args.type == type_get<BoxCollider2D>() && entity_has<CircleCollider>(args.entity))
        {
            entity_remove<CircleCollider>(args.entity); 
        }
        else if (args.type == type_get<CircleCollider>() && entity_has<BoxCollider2D>(args.entity))
        {
            entity_remove<BoxCollider2D>(args.entity);
        }
        
        return ListenerAction::StayListening;
    }

    ListenerAction on_component_removed(const ComponentRemovedArgs& args)
    {
        if (args.type == type_get<Rigidbody2D>())
        {
            auto& rb = entity_get<Rigidbody2D>(args.entity); 

            world()->DestroyBody((b2Body*) rb.body_ptr);
            rb.body_ptr = nullptr;
            
            if (entity_has<BoxCollider2D>(args.entity))
            {
                entity_get<BoxCollider2D>(args.entity).fixture_ptr = nullptr;
            }
            else if (entity_has<CircleCollider>(args.entity))
            {
                entity_get<CircleCollider>(args.entity).fixture_ptr = nullptr;
            } 
        }
        else if (args.type == type_get<BoxCollider2D>())
        {
            auto& collider = entity_get<BoxCollider2D>(args.entity);
            auto& name = entity_get<Name>(args.entity);
            
            if (entity_has<Rigidbody2D>(args.entity))
            {
                auto& rb = entity_get<Rigidbody2D>(args.entity);
                if (!rb.body_ptr || !collider.fixture_ptr)
                {
                    return ListenerAction::StayListening;
                }

                ((b2Body*) rb.body_ptr)->DestroyFixture((b2Fixture*) collider.fixture_ptr);
            }
        }
        else if (args.type == type_get<CircleCollider>())
        {
            auto& collider = entity_get<CircleCollider>(args.entity);
            if (entity_has<Rigidbody2D>(args.entity))
            {
                auto& rb = entity_get<Rigidbody2D>(args.entity);
                if (!rb.body_ptr || !collider.fixture_ptr)
                {
                    return ListenerAction::StayListening;
                }

                ((b2Body*) rb.body_ptr)->DestroyFixture((b2Fixture*) collider.fixture_ptr);
            }
        }
        return ListenerAction::StayListening;
    }
    
    ListenerAction fixed_update()
    {
        world()->SetGravity((const b2Vec2&) physics_2d->gravity);
        world()->Step(fixed_delta_seconds(), physics_2d->velocity_iterations, physics_2d->position_iterations);
        update_bodies();
        return ListenerAction::StayListening;
    }
}