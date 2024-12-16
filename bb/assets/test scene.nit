AssetInfo:
  type: Scene
  name: test scene
  path: test scene.nit
  id: 9621028665531299668
  version: 0
Scene:
  Entities:
    Entity:
      Name:
        data: camera
      Transform:
        position: [0, 0, 2.95000005]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Camera:
        projection: Orthographic
        fov: 100
        near_clip: 0.100000001
        far_clip: 1000
        size: 9.19999981
    Entity:
      Name:
        data: Player
      Transform:
        position: [0, 0, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Sprite:
        texture: [ShipTexture, Texture2D, 16847963267852121044]
        sub_texture: ""
        visible: true
        tint: [1, 1, 1, 1]
        size: [1, 1]
        flip_x: false
        flip_y: false
        tiling_factor: [1, 1]
        keep_aspect: true
        draw_layer: 0
      Movement:
        speed: [0, 0]
      Health:
        max: 1
        current: 1
      Player:
        firerate_gun_1: 1
        firerate_gun_2: 1
        firerate_gun_3: 1
        firerate_gun_max: 1
        firerate_sword_1: 1
        firerate_sword_2: 1
        firerate_sword_3: 1
        firerate_sword_max: 1