#include "physics_2d.h"
#include "box_collider_2d.h"
#include "circle_collider.h"
#include "physic_material.h"
#include "rigidbody_2d.h"
#include "trigger_events.h"
#include "box2d/box2d.h"
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

    static b2WorldId to_box2d(const WorldHandle& handle)
    {
        return (const b2WorldId&) handle;
    }

    static b2BodyId to_box2d(const BodyHandle& handle)
    {
        return (const b2BodyId&) handle;
    }

    static b2ShapeId to_box2d(const ShapeHandle& handle)
    {
        return (const b2ShapeId&) handle;
    }

    static b2Vec2 to_box2d(const Vector2& vec)
    {
        return (const b2Vec2&) vec;
    }

    static b2BodyType to_box2d(BodyType type)
    {
        return (b2BodyType) type;
    }
    
    static WorldHandle from_box2d(const b2WorldId& handle)
    {
        return (const WorldHandle&) handle;
    }

    static BodyHandle from_box2d(const b2BodyId& handle)
    {
        return (const BodyHandle&) handle;
    }

    static ShapeHandle from_box2d(const b2ShapeId& handle)
    {
        return (const ShapeHandle&) handle;
    }

    static Vector2 from_box2d(const b2Vec2& vec)
    {
        return (const Vector2&) vec;
    }

    static BodyType from_box2d(b2BodyType type)
    {
        return (BodyType) type;
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

        u32 entity_count = entity_registry_get_instance()->max_entities;

        physics_2d->all_entity_ids = new EntityID[entity_count];

        for (EntityID i = 0; i < entity_count; ++i)
        {
            physics_2d->all_entity_ids[i] = i;
        }

        entity_create_group<Transform, Rigidbody2D>();
        entity_create_group<Transform, Rigidbody2D, BoxCollider2D>();
        entity_create_group<Transform, Rigidbody2D, CircleCollider>();
        entity_create_group<TriggerEvents>();
        
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

        if (auto world_id = to_box2d(physics_2d->world_handle); b2World_IsValid(world_id))
        {
            b2DestroyWorld(world_id);
            physics_2d->world_handle = {};
        }
    }

    void rigidbody_add_force(Rigidbody2D& rb, const Vector2& force, const Vector2& point)
    {
        if (b2BodyId body_id = to_box2d(rb.handle); b2Body_IsValid(body_id))
        {
            b2Body_ApplyForce(body_id, to_box2d(force), to_box2d(point), false);
        }
    }

    void rigidbody_set_velocity(Rigidbody2D& rb, const Vector2& velocity)
    {
        if (b2BodyId body_id = to_box2d(rb.handle); b2Body_IsValid(body_id))
        {
            b2Body_SetLinearVelocity(body_id, to_box2d(velocity));
        }
    }

    void rigidbody_set_angular_velocity(Rigidbody2D& rb, f32 velocity)
    {
        if (b2BodyId body_id = to_box2d(rb.handle); b2Body_IsValid(body_id))
        {
            b2Body_SetAngularVelocity(body_id, velocity);
        }
    }

    static b2WorldId world()
    {
        if (!physics_2d_has_instance())
        {
            NIT_CHECK(false);
            return {};
        }

        if (auto world = to_box2d(physics_2d->world_handle); !b2World_IsValid(world))
        {
            b2WorldDef def = b2DefaultWorldDef();
            def.gravity = to_box2d(physics_2d->gravity);
            physics_2d->world_handle = from_box2d(b2CreateWorld(&def));
            b2World_EnableSleeping(to_box2d(physics_2d->world_handle), false);
        }

        return to_box2d(physics_2d->world_handle);
    }

    static void rigidbody_invalidate(Rigidbody2D& rb, const Vector2& position, f32 angle)
    {
        if (b2Body_IsValid(to_box2d(rb.handle)))
        {
            b2DestroyBody(to_box2d(rb.handle));
            rb.handle = {};
        }
        
        b2BodyDef def    = b2DefaultBodyDef();
        def.type         = to_box2d(rb.body_type);
        def.position     = to_box2d(position);
        def.rotation     = b2MakeRot(angle);
        def.isAwake      = true;
        def.gravityScale = rb.gravity_scale;
        
        rb.handle      = from_box2d(b2CreateBody(world(), &def));
    }

    static void shape_def_init(b2ShapeDef& def, AssetHandle& material_handle, bool is_sensor)
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

        def             = b2DefaultShapeDef();
        def.density     = physic_material->density;
        def.density     = physic_material->density;
        def.friction    = physic_material->friction;
        def.restitution = physic_material->bounciness;
        def.isSensor    = is_sensor;
    }
    
    static void box_collider_invalidate(EntityID entity, Rigidbody2D& rb, BoxCollider2D& collider)
    {
        auto body = to_box2d(rb.handle);
        
        if (!b2Body_IsValid(body))
        {
            NIT_CHECK(false);
            return;
        }

        auto shape = to_box2d(collider.handle);
        
        if (b2Shape_IsValid(shape))
        {
            b2DestroyShape(shape, true);
        }

        b2ShapeDef def;
        shape_def_init(def, collider.physic_material, collider.is_trigger);
        b2Polygon poly = b2MakeBox(collider.size.x / 2.f, collider.size.y / 2.f);
        b2ShapeId shape_id = b2CreatePolygonShape(body, &def, &poly);
        b2Shape_SetUserData(shape_id, &physics_2d->all_entity_ids[entity]);
        collider.handle = from_box2d(shape_id);
    }
    
    static void circle_collider_invalidate(EntityID entity, Rigidbody2D& rb, CircleCollider& collider)
    {
        auto body = to_box2d(rb.handle);
        
        if (!b2Body_IsValid(body))
        {
            NIT_CHECK(false);
            return;
        }

        auto shape = to_box2d(collider.handle);
        
        if (b2Shape_IsValid(shape))
        {
            b2DestroyShape(shape, true);
        }

        b2ShapeDef def;
        shape_def_init(def, collider.physic_material, collider.is_trigger);
        b2Circle circle {
            .center = to_box2d(collider.center),
            .radius = collider.radius
        };
        
        b2ShapeId shape_id = b2CreateCircleShape(body, &def, &circle);
        b2Shape_SetUserData(shape_id, &physics_2d->all_entity_ids[entity]);
        collider.handle = from_box2d(shape_id);
    }

    static void physics_entity_invalidate(EntityID entity)
    {
        auto& rb = entity_get<Rigidbody2D>(entity);

        if (!rb.invalidated)
        {
            auto& transform = entity_get<Transform>(entity);
                
            rigidbody_invalidate(rb, (const Vector2&) transform.position, transform.rotation.z);
            rb.invalidated = true;

            if (entity_has<BoxCollider2D>(entity))
            {
                entity_get<BoxCollider2D>(entity).invalidated = false;
            }

            if (entity_has<CircleCollider>(entity))
            {
                entity_get<CircleCollider>(entity).invalidated = false;
            }
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

        if (args.type == type_get<Rigidbody2D>())
        {
            physics_entity_invalidate(args.entity);
        }
        
        return ListenerAction::StayListening;
    }

    ListenerAction on_component_removed(const ComponentRemovedArgs& args)
    {   
        if (args.type == type_get<Rigidbody2D>())
        {
            auto& rb = entity_get<Rigidbody2D>(args.entity);
            
            if (b2Body_IsValid(to_box2d(rb.handle)))
            {
                b2DestroyBody(to_box2d(rb.handle));
            }
        }
        else if (args.type == type_get<BoxCollider2D>())
        {
            auto& collider = entity_get<BoxCollider2D>(args.entity);
            
            if (entity_has<Rigidbody2D>(args.entity))
            {
                auto& rb = entity_get<Rigidbody2D>(args.entity);
                
                if (!b2Body_IsValid(to_box2d(rb.handle)) || !b2Shape_IsValid(to_box2d(collider.handle)))
                {
                    return ListenerAction::StayListening;
                }

                b2DestroyShape(to_box2d(collider.handle), true);
            }
        }
        else if (args.type == type_get<CircleCollider>())
        {
            auto& collider = entity_get<CircleCollider>(args.entity);
            
            if (entity_has<Rigidbody2D>(args.entity))
            {
                auto& rb = entity_get<Rigidbody2D>(args.entity);
                
                if (!b2Body_IsValid(to_box2d(rb.handle)) || !b2Shape_IsValid(to_box2d(collider.handle)))
                {
                    return ListenerAction::StayListening;
                }

                b2DestroyShape(to_box2d(collider.handle), true);
            }
        }
        return ListenerAction::StayListening;
    }
    
    ListenerAction fixed_update()
    {
        //TODO: check groups before start ticking this

        NIT_IF_EDITOR_ENABLED(if ((editor_get_instance()->is_paused && !editor_get_instance()->next_frame) || editor_get_instance()->is_stopped) return ListenerAction::StayListening;)
        
        for (EntityID entity : entity_get_group<Transform, Rigidbody2D>().entities)
        {
            if (!entity_global_enabled(entity))
            {
                continue;
            }
            
            physics_entity_invalidate(entity);
        }
        
        for (EntityID entity : entity_get_group<Transform, Rigidbody2D, BoxCollider2D>().entities)
        {
            if (!entity_global_enabled(entity))
            {
                continue;
            }
            
            auto& collider = entity_get<BoxCollider2D>(entity);

            if (!collider.invalidated)
            {
                auto& rb = entity_get<Rigidbody2D>(entity);
                
                box_collider_invalidate(entity, rb, collider);
                collider.invalidated = true;
            }
        }

        for (EntityID entity : entity_get_group<Transform, Rigidbody2D, CircleCollider>().entities)
        {
            if (!entity_global_enabled(entity))
            {
                continue;
            }
            
            auto& collider = entity_get<CircleCollider>(entity);

            if (!collider.invalidated)
            {
                auto& rb= entity_get<Rigidbody2D>(entity);
                
                circle_collider_invalidate(entity, rb, collider);
                collider.invalidated = true;
            }
        }

        b2World_SetGravity(world() , to_box2d(physics_2d->gravity));
        b2World_Step(world(), fixed_delta_seconds(), physics_2d->sub_steps);

        for (EntityID entity : entity_get_group<TriggerEvents>().entities)
        {
            if (!entity_global_enabled(entity))
            {
                continue;
            }
            
            auto& events = entity_get<TriggerEvents>(entity);
            events.enter_events.clear();
            events.exit_events.clear();
        }
        
        b2SensorEvents sensor_events = b2World_GetSensorEvents(to_box2d(physics_2d->world_handle));

        for (i32 i = 0; i < sensor_events.beginCount; ++i)
        {
            b2SensorBeginTouchEvent* begin_touch = sensor_events.beginEvents + i;
            
            EntityID visitor_entity = *(EntityID*) b2Shape_GetUserData(begin_touch->visitorShapeId);
            EntityID trigger_entity = *(EntityID*) b2Shape_GetUserData(begin_touch->sensorShapeId);

            TriggerEnterEvent enter_event = {
                .trigger_entity = trigger_entity,
                .visitor_entity = visitor_entity
            };
            
            if (entity_has<TriggerEvents>(trigger_entity))
            {
                entity_get<TriggerEvents>(trigger_entity).enter_events.emplace_back(enter_event);
            }

            if (entity_has<TriggerEvents>(visitor_entity))
            {
                entity_get<TriggerEvents>(visitor_entity).enter_events.emplace_back(enter_event);
            }
        }

        for (i32 i = 0; i < sensor_events.endCount; ++i)
        {
            b2SensorEndTouchEvent* end_touch = sensor_events.endEvents + i;
            
            EntityID visitor_entity = *(EntityID*) b2Shape_GetUserData(end_touch->visitorShapeId);
            EntityID trigger_entity = *(EntityID*) b2Shape_GetUserData(end_touch->sensorShapeId);

            TriggerExitEvent exit_event = {
                .trigger_entity = trigger_entity,
                .visitor_entity = visitor_entity
            };
            
            if (entity_has<TriggerEvents>(trigger_entity))
            {
                entity_get<TriggerEvents>(trigger_entity).exit_events.emplace_back(exit_event);
            }

            if (entity_has<TriggerEvents>(visitor_entity))
            {
                entity_get<TriggerEvents>(visitor_entity).exit_events.emplace_back(exit_event);
            }
        }
        
        for (EntityID entity : entity_get_group<Transform, Rigidbody2D>().entities)
        {
            if (!entity_global_enabled(entity))
            {
                continue;
            }
            
            const bool has_box_collider    = entity_has<BoxCollider2D>(entity); 
            const bool has_circle_collider = entity_has<CircleCollider>(entity); 
            
            if (!has_box_collider && !has_circle_collider)
            {
                continue;
            }
            
            auto& rb = entity_get<Rigidbody2D>(entity);
            auto& transform = entity_get<Transform>(entity);  
            
            auto body = to_box2d(rb.handle);
        
            if (!b2Body_IsValid(body))
            {
                NIT_CHECK(false);
                continue;
            }

            if (rb.enabled && !b2Body_IsEnabled(body))
            {
                b2Body_Enable(body);
            }
            else if (!rb.enabled && b2Body_IsEnabled(body))
            {
                b2Body_Disable(body);
            }

            if (rb.follow_transform)
            {
                b2Body_SetTransform(body, to_box2d((const Vector2&) transform.position), b2MakeRot(to_radians(transform.rotation.z)));
            }
            else
            {
                Vector2 center;
                
                if (has_box_collider)
                {
                    center = entity_get<BoxCollider2D>(entity).center;
                }
                else if (has_circle_collider)
                {
                    center = entity_get<CircleCollider>(entity).center;
                }
                else
                {
                    NIT_CHECK(false);
                    continue;
                }
                
                Vector2 body_pos = from_box2d(b2Body_GetPosition(body)) - center;
                transform.position = { body_pos.x, body_pos.y, transform.position.z };
                const auto rot = b2Body_GetRotation(body);
                transform.rotation.z = to_degrees(atan2(rot.s, rot.c));
            }

            b2Body_SetGravityScale(body, rb.gravity_scale);
        }
        
        return ListenerAction::StayListening;
    }
}