AssetInfo:
  type: Scene
  name: PresetsScene
  path: PresetsScene.nit
  id: 16705672459133264023
  version: 0
Scene:
  Entities:
    Entity:
      Enabled: true
      Name: BulletPreset
      UUID: 14570836660667367363
      Transform:
        position: [-3.7252903e-09, 1, 0]
        rotation: [0, 0, 0]
        scale: [0.200000003, 0.200000003, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: false
        tint: [1, 0.399193525, 0.399193525, 1]
        size: [1, 1]
        flip_x: false
        flip_y: false
        tiling_factor: [1, 1]
        keep_aspect: true
        draw_layer: 0
      Rigidbody2D:
        enabled: false
        body_type: Dynamic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: false
        radius: 0.150000006
        center: [0, 0]
        physic_material: ["", "", 0]
      Movement:
        speed: [3, 3]
      Bullet:
        {}
    Entity:
      Enabled: true
      Name: HittablePreset
      UUID: 6974000574263080879
      Transform:
        position: [0.0158021152, 2.02931023, 2.00006104]
        rotation: [0, 0, 0]
        scale: [0.200000003, 0.200000003, 1]
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
        body_type: Kinematic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: true
        radius: 0.150000006
        center: [0, 0]
        physic_material: ["", "", 0]
      TriggerEvents:
        {}
      Health:
        max: 100
        current: 100
      Hittable:
        {}
    Entity:
      Enabled: false
      Name: HomingMissilePreset
      UUID: 5583845142476208914
      Transform:
        position: [0, 0, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Circle:
        visible: true
        tint: [0.213709652, 0.790745318, 1, 1]
        radius: 0.150000006
        thickness: 1
        fade: 0.00999999978
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: true
        radius: 0.150000006
        center: [0, 0]
        physic_material: ["", "", 0]
      CollisionCategory:
        collision_flags: [GameCollisionFlags, CollisionFlags, 12590185099395023690]
        name: ENEMY
      Health:
        max: 100
        current: 100
      Hittable:
        {}
      HomingMissile:
        distance_to_attack: 1
        wait_time: 0.5
        approach_speed: 3
        attack_speed: 6