red_rough = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
green_rough = gr.material_cook_torrance({0.1, 0.7, 0.1}, {0.01, 0.01, 0.01}, 0.9)
blue_rough = gr.material_cook_torrance({0.1, 0.1, 0.7}, {0.01, 0.01, 0.01}, 0.9)
white_rough = gr.material_cook_torrance({1.0, 1.0, 1.0}, {0.01, 0.01, 0.01}, 0.9)
yellow_rough = gr.material_cook_torrance({1.0, 1.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)
black_rough = gr.material_cook_torrance({0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)

white_reflective = gr.material_reflective({1.0, 1.0, 1.0}, 0.1)
green_reflective = gr.material_reflective({0.1, 1.0, 0.1}, 0.1)

white_refractive = gr.material_refractive({1.0, 1.0, 1.0}, 0.05, 2.0)
blue_refractive = gr.material_refractive({0.3, 0.3, 1.0}, 0.05, 2.0)

normal_map = gr.uv_map("./textures/normal_map_example.png")

mat_normal_cube = gr.material_cook_torrance({1, 1, 1}, {0.01, 0.01, 0.01}, 0.2)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)

scene = gr.node('scene')
scene:scale(10, 10 , 10)

-- The Box
-- the floor
floor = gr.mesh( 'floor', 'plane.obj' )
scene:add_child(floor)
floor:set_material(white_rough)
floor:scale(10, 1, 10)

-- left
left = gr.mesh( 'left', 'plane.obj' )
scene:add_child(left)
left:set_material(green_rough)
left:scale(10, 1, 10)
left:rotate('Z', -90)
left:translate(-5, 10, 0)

-- right
right = gr.mesh( 'right', 'plane.obj' )
scene:add_child(right)
right:set_material(red_rough)
right:scale(10, 1, 10)
right:rotate('Z', 90)
right:translate(5, 0, 0)

-- ceil
ceil = gr.mesh( 'ceil', 'plane.obj' )
scene:add_child(ceil)
ceil:set_material(white_rough)
ceil:scale(10, 1, 10)
ceil:rotate('Z', 180)
ceil:translate(0, 10, 0)

-- back
back = gr.mesh( 'back', 'plane.obj' )
scene:add_child(back)
back:set_material(white_rough)
back:scale(10, 1, 10)
back:rotate('X', 90)
back:translate(0, 0, -10)

-- front
front = gr.mesh( 'front', 'plane.obj' )
scene:add_child(front)
front:set_material(black_rough)
front:scale(10, 1, 10)
front:rotate('X', -90)
front:translate(0, 5, 10)


-- The Stuff
normalCube = gr.cube('normalCube')
scene:add_child(normalCube)
normalCube:set_material(mat_normal_cube)
normalCube:scale(3, 3, 3)
normalCube:rotate('Y', 45)
normalCube:translate(-2, 4.0, -3)

-- The lights
l1 = gr.light({0, 90, -20}, {0.95, 0.85, 0.7}, {1, 0, 0})
l2 = gr.light({25, 70, -20}, {0.95, 0.85, 0.7}, {1, 0, 0})
l3 = gr.light({-25, 50, -10}, {0.95, 0.85, 0.7}, {1, 0, 0})

gr.render(scene, 'cornell-box-normal.png', 512, 512,
	  {0, 50, 80}, {0, 50, 7}, {0, 1, 0}, 60,
	  {0.4, 0.4, 0.4}, {l1, l2, l3}, 3, false, 2, 100, 4, false, 2)
