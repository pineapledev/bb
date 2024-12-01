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
        data: ground
      Transform:
        position: [0.48703897, -1.5412848, 0]
        rotation: [0, 0, 0]
        scale: [7.25, 1, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: true
        tint: [0.252178699, 0.661568642, 0.947580636, 1]
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
      BoxCollider2D:
        is_trigger: false
        size: [7.25, 1]
        center: [0, 0]
        physic_material: ["", "", 0]
    Entity:
      Name:
        data: red
      Transform:
        position: [-1.12852049, -0.526285172, 0]
        rotation: [0, 0, 1.98791622e-06]
        scale: [1, 1, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: true
        tint: [1, 0.5, 0.5, 1]
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
      BoxCollider2D:
        is_trigger: false
        size: [1, 1]
        center: [0, 0]
        physic_material: ["", "", 0]
    Entity:
      Name:
        data: 16
      Transform:
        position: [1.05638742, -0.526384413, 0]
        rotation: [0, 0, -90.0006256]
        scale: [0.999974549, 0.999974549, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: true
        tint: [1, 0.504032254, 0.504032254, 1]
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
      BoxCollider2D:
        is_trigger: false
        size: [1, 1]
        center: [0, 0]
        physic_material: ["", "", 0]
    Entity:
      Name:
        data: 17
      Transform:
        position: [-0.120509334, -0.536320686, 0]
        rotation: [0, 0, -12.4667654]
        scale: [0.999999344, 0.999999344, 1]
      Circle:
        visible: true
        tint: [0.520161271, 0.976782024, 1, 1]
        radius: 0.5
        thickness: 1.70000005
        fade: 0.00999999978
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 1
      CircleCollider:
        is_trigger: false
        radius: 0.5
        center: [0, 0]
        physic_material: ["", "", 0]
    Entity:
      Name:
        data: 18
      Transform:
        position: [0.276211798, 0.266167313, 0]
        rotation: [0, 0, -14.097662]
        scale: [1, 1, 1]
      Circle:
        visible: true
        tint: [0.900233984, 0.524193525, 1, 1]
        radius: 0.400000006
        thickness: 1.20000005
        fade: 0.00999999978
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 1
      CircleCollider:
        is_trigger: false
        radius: 0.400000006
        center: [0, 0]
        physic_material: ["", "", 0]
    Entity:
      Name:
        data: 19
      Transform:
        position: [0.779976487, 0.676965773, 0]
        rotation: [0, 0, -210.088409]
        scale: [2.79892731, 0.418984711, 1]
      Sprite:
        texture: ["", "", 0]
        sub_texture: ""
        visible: true
        tint: [0.503433764, 0.834677398, 0.471188843, 1]
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
      BoxCollider2D:
        is_trigger: false
        size: [2.79999995, 0.400000006]
        center: [0, 0]
        physic_material: ["", "", 0]