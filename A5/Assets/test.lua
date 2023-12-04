-- A simple scene with some miscellaneous geometry.
mat1  = gr.material({0.7, 0.6, 1.0}, {0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)	
mat2  = gr.material({1.0, 0.6, 0.1}, {1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 250)

mirror = gr.material( {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25)
glass = gr.material({0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7},  25)

white = gr.material({0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25)
red = gr.material({0.44, 0.137, 0.1886}, {0.44, 0.137, 0.1886}, {0.3, 0.05, 0.05}, 25)
blue = gr.material({0.1386, 0.1327, 0.45}, {0.1386, 0.1327, 0.45}, {0.1, 0.05, 0.5}, 25)
grey= gr.material({0.4649, 0.4823, 0.5002}, {0.4649, 0.4823, 0.5002}, {0.3, 0.3, 0.3}, 25)

scene = gr.node('root')

cylinder = gr.cylinder('cylinder')
cylinder:scale(3.5, 5, 3.5)
cylinder:translate(38, 23, 92)
cylinder:set_material(verti)
scene:add_child(cylinder)

cone = gr.cone("cone")
cone:scale(5.5, 10, 5.5)
cone:translate(50, 19, 73)
cone:set_material(horizontalPerlinTexture)
scene:add_child(cone)

floor = gr.plane('floor')
floor:scale(100, 1, 100)
floor:translate(-15, -2, -15)
floor:set_material(grey)
scene:add_child(floor)

backwall = gr.plane('backwall')
backwall:rotate('x', 90)
backwall:scale(100, 100, 1)
backwall:translate(-15, 30, -12.5)
backwall:set_material(grey)
scene:add_child(backwall)

leftwall = gr.plane("leftwall")
leftwall:rotate('z', -90)
leftwall:scale(1, 100, 100)
leftwall:translate(-13.5, 35, -13.5)
leftwall:set_material(red)
scene:add_child(leftwall)

rightwall = gr.plane('rightwall')
rightwall:rotate('z', 90)
rightwall:scale(1, 100, 100)
rightwall:translate(13.5, -5, -13.5)
rightwall:set_material(blue)
scene:add_child(rightwall)

ceiling = gr.plane('ceiling')
ceiling:rotate('x', 180)
ceiling:scale(100, 1, 100)
ceiling:translate(-15, 24.5, 30)
ceiling:set_material(grey)
scene:add_child(ceiling)

white_light = gr.light({-2, 21.0, 8}, {1, 0, 0}, {0, 0, 1}, 2, 2, {0.9, 0.9, 0.9}, {1, 0, 0})

lightpanel = gr.nh_plane('lightpanel', {-2, 21.0, 8}, {0, -1, 0}, 6)
lightpanel:set_material(white)
lightpanel:set_illumination(white_light)
scene:add_child(lightpanel)

gr.render(scene, 'test.png', 512, 512,
	  {0, 11, 40}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})