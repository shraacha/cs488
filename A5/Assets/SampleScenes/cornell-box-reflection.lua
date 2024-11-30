-- test for hierarchical ray-tracers.

red_rough = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
green_rough = gr.material_cook_torrance({0.1, 0.7, 0.1}, {0.01, 0.01, 0.01}, 0.9)
blue_rough = gr.material_cook_torrance({0.1, 0.1, 0.7}, {0.01, 0.01, 0.01}, 0.9)
white_rough = gr.material_cook_torrance({1.0, 1.0, 1.0}, {0.01, 0.01, 0.01}, 0.9)
black_rough = gr.material_cook_torrance({0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)

white_reflective = gr.material_reflective({1.0, 1.0, 1.0}, 0.1)
green_reflective = gr.material_reflective({0.1, 1.0, 0.1}, 0.1)

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
sphere = gr.sphere('sphere')
scene:add_child(sphere)
sphere:set_material(white_reflective)
sphere:scale(1.5, 1.5, 1.5)
sphere:translate(1.7, 1.5, -2)

cube = gr.cube('cube')
scene:add_child(cube)
cube:set_material(white_reflective)
cube:scale(2.5, 6, 2.5)
cube:rotate('Y', 20)
cube:translate(-3, 0, -6)

-- The lights
l1 = gr.light({0, 90, -20}, {0.95, 0.85, 0.7}, {1, 0, 0})

gr.render(scene, 'cornell-box-reflection.png', 256, 256,
	  {0, 50, 80}, {0, 50, 7}, {0, 1, 0}, 60,
	  {0.4, 0.4, 0.4}, {l1}, 3, 2)
