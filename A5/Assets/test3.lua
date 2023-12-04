mat1  = gr.material({0.7, 0.6, 1.0}, {0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)	
mat2  = gr.material({1.0, 0.6, 0.1}, {1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 250)


purple = gr.material({0.56, 0.50, 0.91}, {0.56, 0.50, 0.91}, {0.5, 0.4, 0.8}, 25)
white = gr.material({0.73, 0.73, 0.73}, {0.73, 0.73, 0.73}, {0.1, 0.1, 0.1}, 25)
glass = gr.material({0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7},  25)

blue = gr.material({0.1386, 0.1327, 0.65}, {0.1386, 0.1327, 0.65}, {0.1, 0.05, 0.5}, 25)
red = gr.material({0.65, 0.05, 0.05}, {0.65, 0.05, 0.05}, {0.1, 0.1, 0.1}, 25)
green = gr.material({0.24, 0.85, 0.3}, {0.24, 0.85, 0.3}, {0.1, 0.1, 0.1}, 25)
grey = gr.material({0.4649, 0.4823, 0.5002}, {0.4649, 0.4823, 0.5002}, {0.3, 0.3, 0.3}, 25)
backgrey = gr.material({0.44, 0.44, 0.44}, {0.44, 0.44, 0.44}, {0.3, 0.3, 0.3}, 25)
floorgrey = gr.material({0.38, 0.38, 0.38}, {0.38, 0.38, 0.38}, {0.3, 0.3, 0.3}, 25)
pyramid = gr.material({1, 1, 0.39}, {1, 1, 0.39}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')

reflective_sphere = gr.sphere('sphere1')
reflective_sphere:scale(5, 5, 5)
reflective_sphere:translate(-11, -4, -10.5)
reflective_sphere:set_material(purple)
scene:add_child(reflective_sphere)

box = gr.cube('box')
box:rotate('y', 30)
box:scale(7, 7, 7)
box:translate(7, -8.75, -9)
box:set_material(green)
scene:add_child(box)

floor = gr.cube('floor')
floor:scale(200, 1, 200)
floor:translate(-50, -10, -60)
scene:add_child(floor)
floor:set_material(floorgrey)

backwall = gr.cube('backwall')
backwall:scale(200, 200, 1)
backwall:translate(-50, -50, -35)
backwall:set_material(backgrey)
scene:add_child(backwall)

ceiling = gr.cube('ceiling')
ceiling:scale(200, 1, 200)
ceiling:translate(-25, 30, -35)
ceiling:set_material(grey)
scene:add_child(ceiling)

-- light = gr.nh_sphere('light', {0, 7, 8}, 1)
-- light:set_material(mat1)
-- scene:add_child(light)

white_light = gr.light({-6, 30.0, -14}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 15, 15, 15, {125, 125, 125}, {1, 0, 0})
-- white_light = gr.light({-6, 30.0, -14}, {1, 0, 0}, {0, 0, 1}, 15, 15, {1, 1, 1}, {1, 0, 0})
lightpanel = gr.nh_box('lightpanel', {-6, 30.0, -14}, {0, -1, 0}, 15)
lightpanel:set_material(white)
lightpanel:set_illumination(white_light)
scene:add_child(lightpanel)


-- lightpanel = gr.nh_box('lightpanel', {-2, 18.0, -6}, {0, -1, 0}, 6)
-- lightpanel:set_material(white)
-- lightpanel:set_illumination(white_light)
-- scene:add_child(lightpanel)

gr.render(scene, 'test3.png', 512, 512,
	  {1, 12, 48}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
