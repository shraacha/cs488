-- A simple scene with some miscellaneous geometry.

mat1 = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 0.1)
mat2 = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 0.2)
mat3 = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 0.4)
mat4 = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 0.6)
mat5 = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 0.8)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {-200, 0, -400}, 50)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {-100, 0, -400}, 50)
scene_root:add_child(s2)
s2:set_material(mat2)

s3 = gr.nh_sphere('s3', {0, 0, -400}, 50)
scene_root:add_child(s3)
s3:set_material(mat3)

s4 = gr.nh_sphere('s4', {100, 0, -400}, 50)
scene_root:add_child(s4)
s4:set_material(mat4)

s5 = gr.nh_sphere('s5', {200, 0, -400}, 50)
scene_root:add_child(s5)
s5:set_material(mat5)

white_light = gr.light({0, 200, 200}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'sphere-roughness-test.png', 512, 512,
	  {0, 0, 400}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
