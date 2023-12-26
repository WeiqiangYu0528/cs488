kuribo_mat = gr.material( {0.25, 0.25, 0.25}, {0, 0, 0}, {0.25, 0.25, 0.25}, 25)

red_wall = gr.material( {0.44, 0.137, 0.1886}, {0.44, 0.137, 0.1886}, {0.3, 0.05, 0.05}, 25)
blue_wall = gr.material({0.1386, 0.1327, 0.45},  {0.1386, 0.1327, 0.45}, {0.1, 0.05, 0.5}, 25)
grey_wall = gr.material( {0.3, 0.3, 0.3}, {0.4649, 0.4823, 0.5002}, {0.3, 0.3, 0.3}, 25)

scene = gr.node('scene')

kuribo = gr.mesh('kuribo', 'Kuribo.obj')
kuribo:translate(0, -8, 0)
kuribo:set_material(kuribo_mat)
kuribo:set_image()
scene:add_child(kuribo)

floor = gr.cube('floor')
floor:scale(100, 1, 100)
floor:translate(-25, -10, -30)
scene:add_child(floor)
floor:set_material(grey_wall)

backwall = gr.cube('backwall')
backwall:scale(100, 100, 1)
backwall:translate(-20, -12, -20)
backwall:set_material(grey_wall)
scene:add_child(backwall)

leftwall = gr.cube('leftwall')
leftwall:scale(1, 100, 100)
leftwall:translate(-15, -15, -30)
leftwall:set_material(red_wall)
scene:add_child(leftwall)

rightwall = gr.cube('rightwall')
rightwall:scale(1, 100, 100)
rightwall:translate(15, -15, -30)
rightwall:set_material(blue_wall)
scene:add_child(rightwall)

ceiling = gr.cube('ceiling')
ceiling:scale(100, 1, 100)
ceiling:translate(-25, 20, -30)
ceiling:set_material(grey_wall)
scene:add_child(ceiling)

-- light = gr.nh_sphere('light', {0, 7, 8}, 1)
-- light:set_material(mat1)
-- scene:add_child(light)
-- white_light = gr.light({1.5, 8, 35},)
white_light = gr.light({1.5, 8, 35}, {0, 0, 0}, {0, 0, 0}, 1, 1, {1, 1, 1}, {1, 0, 0})



gr.render(scene, 'kuribo.png', 1024, 1024,
	  {1, 5, 35}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
