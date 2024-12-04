-- mats
tex_ball8 = gr.uv_map("./textures/Ball8.png")
mat_ball8 = gr.material_cook_torrance({0.1, 1.0, 0.1}, {0.5, 0.5, 0.5}, 0.3)
mat_ball8:add_albedo_map(tex_ball8)

tex_ball4 = gr.uv_map("./textures/Ball4.png")
mat_ball4 = gr.material_cook_torrance({0.1, 1.0, 0.1}, {0.5, 0.5, 0.5}, 0.3)
mat_ball4:add_albedo_map(tex_ball4)

normal_map = gr.uv_map("./textures/normal_map_example.png")
mat_normal_map = gr.material_cook_torrance({0.5, 1.0, 0.5}, {0.5, 0.5, 0.5}, 0.1)
mat_normal_map:add_normal_map(normal_map)

mat_normal_cube = gr.material_cook_torrance({0.3, 0.3, 0.8}, {0.01, 0.01, 0.01}, 0.7)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)
mat_normal_cube:add_normal_map(normal_map)

red_rough = gr.material_cook_torrance({1.0, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
dark_red_rough = gr.material_cook_torrance({0.6, 0.1, 0.1}, {0.01, 0.01, 0.01}, 0.9)
green_rough = gr.material_cook_torrance({0.1, 0.7, 0.1}, {0.01, 0.01, 0.01}, 0.9)
dark_green_rough = gr.material_cook_torrance({0.1, 0.5, 0.1}, {0.01, 0.01, 0.01}, 0.9)
blue_rough = gr.material_cook_torrance({0.1, 0.1, 0.7}, {0.01, 0.01, 0.01}, 0.9)
white_rough = gr.material_cook_torrance({1.0, 1.0, 1.0}, {0.01, 0.01, 0.01}, 0.9)
black_rough = gr.material_cook_torrance({0.0, 0.0, 0.0}, {0.01, 0.01, 0.01}, 0.9)

white_reflective = gr.material_reflective({1.0, 1.0, 1.0}, 0)
green_reflective = gr.material_reflective({0.1, 1.0, 0.1}, 0.1)
yellow_reflective = gr.material_reflective({0.6, 0.7, 0.1}, 0.1)

white_refractive = gr.material_refractive({1.0, 1.0, 1.0}, 0.05, 2.0)

wood = gr.material_cook_torrance({1.0, 0.7, 0.5}, {0.01, 0.01, 0.01}, 0.7)

scene = gr.node('scene')
scene:scale(10, 10 , 10)
scene:translate(-2, 10, 0)


-- the floor
floor = gr.mesh( 'floor', 'plane.obj' )
scene:add_child(floor)
floor:set_material(dark_red_rough)
floor:scale(30, 1, 30)

-- ceil
ceil = gr.mesh( 'ceil', 'plane.obj' )
scene:add_child(ceil)
ceil:set_material(white_rough)
ceil:scale(30, 1, 30)
ceil:rotate('Z', 180)
ceil:translate(0, 10, 0)

-- back
back = gr.mesh( 'back', 'plane.obj' )
scene:add_child(back)
back:set_material(white_rough)
back:scale(20, 1, 20)
back:rotate('X', 90)
back:translate(0, 0, -20)

-- front
front = gr.mesh( 'front', 'plane.obj' )
scene:add_child(front)
front:set_material(white_rough)
front:scale(30, 1, 30)
front:rotate('X', -90)
front:translate(0, 10, 10)

-- left
left = gr.mesh( 'left', 'plane.obj' )
scene:add_child(left)
left:set_material(green_rough)
left:scale(30, 1, 30)
left:rotate('Z', -90)
left:translate(-20, 10, 0)

-- right
right = gr.mesh( 'right', 'plane.obj' )
scene:add_child(right)
right:set_material(red_rough)
right:scale(30, 1, 30)
right:rotate('Z', 90)
right:translate(20, 0, 0)

-- The Stuff

-- table
table = gr.node('table')
scene:add_child(table)
table:translate(0.5, 4, 0)

tableTop = gr.mesh( 'tableTop', 'plane.obj' )
table:add_child(tableTop)
tableTop:set_material(dark_green_rough)
tableTop:scale(4, 1, 7)

tableBarLeft = gr.cube('tableBarLeft')
table:add_child(tableBarLeft)
tableBarLeft:set_material(wood)
tableBarLeft:scale(0.5, 0.5, 16)
tableBarLeft:translate(-4, 0, -8)

tableBarRight = gr.cube('tableBarRight')
table:add_child(tableBarRight)
tableBarRight:set_material(wood)
tableBarRight:scale(0.5, 0.5, 16)
tableBarRight:translate(4, 0, -8)

tableBarBack = gr.cube('tableBarBack')
table:add_child(tableBarBack)
tableBarBack:set_material(wood)
tableBarBack:scale(8, 0.5, 0.5)
tableBarBack:translate(-4, 0, -8)

tableBarFront = gr.cube('tableBarFront')
table:add_child(tableBarFront)
tableBarFront:set_material(wood)
tableBarFront:scale(8, 0.5, 0.5)
tableBarFront:translate(-4, 0, 8)


-- stuff
ball8 = gr.sphere('ball8')
scene:add_child(ball8)
ball8:set_material(mat_ball8)
ball8:rotate('X', -15)
ball8:rotate('Y', -25)
ball8:scale(0.5, 0.5, 0.5)
ball8:translate(1, 4.5, 4.5)

ball4 = gr.sphere('ball4')
scene:add_child(ball4)
ball4:set_material(mat_ball4)
ball4:rotate('X', -45)
ball4:rotate('Y', 25)
ball4:scale(0.5, 0.5, 0.5)
ball4:translate(-1.5, 4.5, 5.5)

normalCube = gr.cube('normalCube')
scene:add_child(normalCube)
normalCube:set_material(mat_normal_cube)
normalCube:rotate('Y', 45)
normalCube:translate(0.6, 4.0, 0)

ball = gr.sphere('ball')
scene:add_child(ball)
ball:set_material(white_refractive)
ball:scale(0.5, 0.5, 0.5)
ball:translate(-0.5, 4.5, 2.8)

-- pole
pole_node = gr.node('pole_node')
scene:add_child(pole_node)
pole_node:scale(1.3, 1, 1.3)
pole_node:translate(-8, 3, -9.5)

pole = gr.cylinder('pole')
pole_node:add_child(pole)
pole:set_material(yellow_reflective)
pole:scale(0.3, 5, 0.3)

hook1_to_pole = gr.node('hook1_to_pole')
pole_node:add_child(hook1_to_pole)
hook1_to_pole:translate(0.5, 3, 0)

hook1 = gr.cylinder('hook1')
hook1_to_pole:add_child(hook1)
hook1:set_material(yellow_reflective)
hook1:scale(0.1, 0.5, 0.1)
hook1:rotate('Z', -60)

hook2_to_pole = gr.node('hook2_to_pole')
pole_node:add_child(hook2_to_pole)
hook2_to_pole:translate(-0.4, 4, 0)

hook2 = gr.cylinder('hook2')
hook2_to_pole:add_child(hook2)
hook2:set_material(yellow_reflective)
hook2:scale(0.1, 0.5, 0.1)
hook2:rotate('Z', 60)

-- mirror
mirror = gr.mesh( 'mirror', 'plane.obj' )
scene:add_child(mirror)
mirror:set_material(white_reflective)
mirror:scale(8, 1, 2.5)
mirror:rotate('X', 90)
mirror:translate(1.5, 6.5, -19.9)

-- The lights
l1 = gr.light({0, 90, -20}, {0.85, 0.75, 0.6}, {0.3, 0, 0})
l2 = gr.light({0, 90, 100}, {0.85, 0.75, 0.6}, {1, 0, 0})
l3 = gr.light({30, 70, -20}, {0.85, 0.75, 0.6}, {1, 0, 0})

gr.render(scene, 'billiards.png', 512, 512,
	  {-10, 70, 90}, {-5, 60, 7}, {0, 1, 0}, 60,
	  {0.4, 0.4, 0.4}, {l1, l2, l3}, 1, false, 2, 100, 4, false, 2)
