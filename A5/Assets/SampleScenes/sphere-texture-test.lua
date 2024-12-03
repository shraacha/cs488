-- mats
tex_8ball = gr.texture("./textures/Ball8.png")
mat_8ball = gr.material_cook_torrance({0.1, 1.0, 0.1}, {0.5, 0.5, 0.5}, 0.3)
mat_8ball:set_albedo_texture(tex_8ball)

red_rough = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
green_rough = gr.material_cook_torrance({0.1, 0.7, 0.1}, {0.01, 0.01, 0.01}, 0.9)
blue_rough = gr.material_cook_torrance({0.1, 0.1, 0.7}, {0.01, 0.01, 0.01}, 0.9)
white_rough = gr.material_cook_torrance({1.0, 1.0, 1.0}, {0.01, 0.01, 0.01}, 0.9)
black_rough = gr.material_cook_torrance({0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)

white_reflective = gr.material_reflective({1.0, 1.0, 1.0}, 0.1)
green_reflective = gr.material_reflective({0.1, 1.0, 0.1}, 0.1)

scene = gr.node('scene')
scene:scale(10, 10 , 10)

-- The Box
-- the floor
floor = gr.mesh( 'floor', 'plane.obj' )
scene:add_child(floor)
floor:set_material(white_rough)
floor:scale(20, 1, 20)


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
back:scale(20, 1, 20)
back:rotate('X', 90)
back:translate(0, 0, -10)

-- The Stuff
sphere = gr.sphere('sphere')
scene:add_child(sphere)
sphere:set_material(mat_8ball)
sphere:rotate('X', -45)
sphere:rotate('Y', 25)
sphere:scale(1.5, 1.5, 1.5)
sphere:translate(1.7, 1.5, -6)

-- The lights
l1 = gr.light({0, 90, -20}, {0.95, 0.85, 0.7}, {1, 0, 0})

gr.render(scene, 'sphere-texture-test.png', 512, 512,
	  {0, 50, 80}, {0, 50, 7}, {0, 1, 0}, 60,
	  {0.4, 0.4, 0.4}, {l1}, 3, false, 2, 100, 80, true)
