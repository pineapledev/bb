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
        data: quad
      Transform:
        position: [-1.06095123, 0.959213257, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: "null"
        visible: true
        tint: [1, 0.300000012, 0.300000012, 1]
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
        gravity_scale: 1
    Entity:
      Name:
        data: circle
      Transform:
        position: [-0.0318238884, -0.656334341, -6.10351562e-05]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Circle:
        visible: true
        tint: [0.300000012, 1, 0.300000012, 1]
        radius: 0.5
        thickness: 0.400000006
        fade: 0.230000004
    Entity:
      Name:
        data: line
      Transform:
        position: [-1, 0, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Line2D:
        visible: true
        tint: [0, 1, 1, 1]
        start: [0, 0]
        end: [1, 1]
        thickness: 0.0500000007
    Entity:
      Name:
        data: text
      Transform:
        position: [-0.131579012, 0.314851582, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Text:
        font: [default_font, Font, 6758496758947967]
        text: BUENAS
        visible: true
        tint: [0.209157124, 0.810483873, 0.795935631, 1]
        spacing: 1
        size: 1
    Entity:
      Name:
        data: bola
      Transform:
        position: [2.1666348, -1.33121252, 0]
        rotation: [0, 0, 0]
        scale: [1, 1, 1]
      Sprite:
        texture: [test sheet, Texture2D, 12761694976786155438]
        sub_texture: bola
        visible: true
        tint: [1, 1, 1, 1]
        size: [1, 1]
        flip_x: false
        flip_y: false
        tiling_factor: [1, 1]
        keep_aspect: true
        draw_layer: 0
    Entity:
      Name:
        data: cpp
      UUID:
        data: 14255733511512113340
      Transform:
        position: [1.47728443, 0.019834131, 2.00006104]
        rotation: [0, 0, 0]
        scale: [0.999999762, 1, 1]
      Sprite:
        texture: [test sheet, Texture2D, 12761694976786155438]
        sub_texture: cpp
        visible: true
        tint: [1, 1, 1, 1]
        size: [1, 1]
        flip_x: false
        flip_y: false
        tiling_factor: [1, 1]
        keep_aspect: true
        draw_layer: 0