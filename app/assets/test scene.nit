AssetInfo:
  type: Scene
  name: test scene
  path: test scene.nit
  id: 9621028665531299668
  version: 0
Scene:
  Entities:
    Entity:
      Enabled: true
      Name: Entity 1
      UUID: 10294327202972041984
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
      Enabled: true
      Name: Entity 2
      UUID: 9417607216195915237
      Transform:
        position: [-0.854475617, 1.76605392, 0]
        rotation: [0, 0, 51.8273888]
        scale: [0.999991059, 0.999991059, 1]
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
        gravity_scale: 0.00999999978
      BoxCollider2D:
        is_trigger: false
        size: [1, 1]
        center: [0, 0]
        physic_material: [test ph material, PhysicMaterial, 17295311971134077511]
    Entity:
      Enabled: true
      Name: Entity 3
      UUID: 15947148584510532775
      Transform:
        position: [-1.24156487, 0.791888177, -6.10351562e-05]
        rotation: [0, 0, 177.998535]
        scale: [0.999957681, 0.999957681, 1]
      Circle:
        visible: true
        tint: [0.300000012, 1, 0.300000012, 1]
        radius: 0.5
        thickness: 0.400000006
        fade: 0.230000004
      Rigidbody2D:
        enabled: true
        body_type: Dynamic
        mass: 1
        gravity_scale: 0
      CircleCollider:
        is_trigger: false
        radius: 0.5
        center: [0, 0]
        physic_material: [test ph material, PhysicMaterial, 17295311971134077511]
    Entity:
      Enabled: true
      Name: Entity 7
      UUID: 4656731474698907055
      Transform:
        position: [1.71819377, 0.489713788, 2.00006104]
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
      Children:
        Entity:
          Enabled: true
          Name: Entity 6
          UUID: 1006179946922367591
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
          Enabled: true
          Name: Entity 5
          UUID: 14444287044799409921
          Transform:
            position: [0.692284644, -1.44233096, 0]
            rotation: [0, 0, 0]
            scale: [1, 1, 1]
          Text:
            font: [default_font, Font, 6758496758947967]
            text: BUENAS
            visible: true
            tint: [0.209157124, 0.810483873, 0.795935631, 1]
            spacing: 1
            size: 1
          Children:
            Entity:
              Enabled: true
              Name: Entity 4
              UUID: 13802930722523881118
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
      Enabled: true
      Name: Entity 7 (clone)
      UUID: 2274769023169322268
      Transform:
        position: [0, 0, 0]
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
      Children:
        Entity:
          Enabled: true
          Name: Entity 6
          UUID: 3087825785419620970
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
          Enabled: false
          Name: Entity 5
          UUID: 15756250672871856216
          Transform:
            position: [0.692284644, -1.44233096, 0]
            rotation: [0, 0, 0]
            scale: [1, 1, 1]
          Text:
            font: [default_font, Font, 6758496758947967]
            text: BUENAS
            visible: true
            tint: [0.209157124, 0.810483873, 0.795935631, 1]
            spacing: 1
            size: 1
          Children:
            Entity:
              Enabled: true
              Name: Entity 4
              UUID: 6151313414813720658
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