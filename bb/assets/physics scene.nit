AssetInfo:
  type: Scene
  name: physics scene
  path: physics scene.nit
  id: 17954291983631195109
  version: 0
Scene:
  Entities:
    Entity:
      Name:
        data: trigger
      Transform:
        position: [-1.00618839, 1.59500968, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: true
        tint: [1, 1, 1, 1]
        size: [1, 1]
        flip_x: false
        flip_y: false
        tiling_factor: [1, 1]
        keep_aspect: true
        draw_layer: 0
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 0
      BoxCollider2D:
        is_trigger: true
        size: [1, 1]
        center: [0, 0]
        physic_material: ["", "", 0]
      TriggerEvents:
        {}
    Entity:
      Name:
        data: visitor
      Transform:
        position: [1.17210197, 1.6641084, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Circle:
        visible: true
        tint: [1, 1, 1, 1]
        radius: 0.5
        thickness: 1
        fade: 0.00999999978
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: false
        radius: 0.5
        center: [0, 0]
        physic_material: ["", "", 0]