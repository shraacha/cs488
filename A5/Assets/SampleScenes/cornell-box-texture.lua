tex_ball1 = gr.uv_map("./textures/Ball1.png")
mat_ball1 = gr.material_cook_torrance({0.1, 1.0, 0.1}, {0.5, 0.5, 0.5}, 0.3)
mat_ball1:add_albedo_map(tex_ball1)

tex_ball2 = gr.uv_map("./textures/Ball2.png")
mat_ball2 = gr.material_cook_torrance({0.1, 1.0, 0.1}, {0.5, 0.5, 0.5}, 0.2)
mat_ball2:add_albedo_map(tex_ball2)

red_rough = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
green_rough = gr.material_cook_torrance({0.1, 0.7, 0.1}, {0.01, 0.01, 0.01}, 0.9)
blue_rough = gr.material_cook_torrance({0.1, 0.1, 0.7}, {0.01, 0.01, 0.01}, 0.9)
white_rough = gr.material_cook_torrance({1.0, 1.0, 1.0}, {0.01, 0.01, 0.01}, 0.9)
black_rough = gr.material_cook_torrance({0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)

white_reflective = gr.material_reflective({1.0, 1.0, 1.0}, 0.1)
green_reflective = gr.material_reflective({0.1, 1.0, 0.1}, 0.1)

white_refractive = gr.material_refractive({1.0, 1.0, 1.0}, 0.05, 2.0)
blue_refractive = gr.material_refractive({0.3, 0.3, 1.0}, 0.05, 2.0)

-- green_rough = gr.material({0.1, 0.7, 0.1}, {0.5, 0.5, 0.5}, 10)
-- white_rough = gr.material({1.0, 1.0, 1.0}, {0.5, 0.5, 0.5}, 10)
-- red_rough = gr.material({1.0, 0.1, 0.1}, {0.5, 0.5, 0.5}, 10)

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
ball1 = gr.sphere('ball1')
scene:add_child(ball1)
ball1:set_material(mat_ball1)
ball1:rotate('X', -15)
ball1:rotate('Y', 25)
ball1:scale(1.5, 1.5, 1.5)
ball1:translate(-2, 4, -3)

ball2 = gr.sphere('ball2')
scene:add_child(ball2)
ball2:set_material(mat_ball2)
ball2:rotate('X', -35)
ball2:rotate('Y', -15)
ball2:scale(1.5, 1.5, 1.5)
ball2:translate(2, 2, -5)

-- The lights
l1 = gr.light({0, 90, -20}, {0.95, 0.85, 0.7}, {1, 0, 0})

gr.render(scene, 'cornell-box-texture.png', 512, 512,
	  {0, 50, 80}, {0, 50, 7}, {0, 1, 0}, 60,
	  {0.4, 0.4, 0.4}, {l1}, 3, false, 2, 30000, 4, false, 2)
