AssetInfo:
  type: Scene
  name: PresetsScene
  path: PresetsScene.nit
  id: 16705672459133264023
  version: 0
Scene:
  Entities:
    Entity:
      Name:
        data: BulletPreset
      UUID:
        data: 14570836660667367363
      Transform:
        position: [0, 1, 0]
        rotation: [0, 0, 0]
        scale: [0.200000003, 0.200000003, 1]
        parent_uuid: 0
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
        body_type: Kinematic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: true
        radius: 0.150000006
        center: [0, 0]
        physic_material: ["", "", 0]
      Movement:
        speed: [3, 3]
      Bullet:
        {}