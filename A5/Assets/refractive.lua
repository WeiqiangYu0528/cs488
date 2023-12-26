mat1  = gr.material({0.7, 0.6, 1.0}, {0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)	
mat2  = gr.material({1.0, 0.6, 0.1}, {1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 250)

glass = gr.material({0.05, 0.05, 0.05}, {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7},  25)
white = gr.material({0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25)
red = gr.material({1, 1, 1}, {0.45, 0.15, 0.2}, {0.25, 0.25, 0.25}, 25)
blue = gr.material({1, 1, 1}, {0.15, 0.15, 0.5}, {0.25, 0.25, 0.25}, 25)
grey= gr.material({1, 1, 1}, {0.5, 0.5, 0.5}, {0.25, 0.25, 0.25}, 25)


scene = gr.node('scene')

transparentSphere = gr.sphere('transparentSphere')
transparentSphere:scale(2.5, 2.5, 2.5)
transparentSphere:translate(1.7, 4.5, 0)
transparentSphere:set_material(glass)
transparentSphere:set_refraction(1, 1.5, 0)
transparentSphere:set_reflection(1, 0)
scene:add_child(transparentSphere)

sphere = gr.sphere('sphere')
sphere:scale(1.5, 1.5, 1.5)
sphere:translate(-2.5, 3.5, -15)
sphere:set_material(mat2)
scene:add_child(sphere)

floor = gr.cube('floor')
floor:scale(100, 1, 100)
floor:translate(-25, -10, -30)
scene:add_child(floor)
floor:set_material(grey)

backwall = gr.cube('backwall')
backwall:scale(100, 100, 1)
backwall:translate(-20, -12, -20)
backwall:set_material(grey)
scene:add_child(backwall)

leftwall = gr.cube('leftwall')
leftwall:scale(1, 100, 100)
leftwall:translate(-15, -15, -30)
leftwall:set_material(red)
scene:add_child(leftwall)

rightwall = gr.cube('rightwall')
rightwall:scale(1, 100, 100)
rightwall:translate(15, -15, -30)
rightwall:set_material(blue)
scene:add_child(rightwall)

ceiling = gr.cube('ceiling')
ceiling:scale(100, 1, 100)
ceiling:translate(-25, 20, -30)
ceiling:set_material(grey)
scene:add_child(ceiling)

white_light = gr.light({1.5, 12, 1}, {0.25, 0, 0}, {0, 0.25, 0}, {0, 0, 0.25}, 2, 2, 2, {1, 1, 1}, {1, 0, 0})
lightpanel = gr.nh_rect('lightpanel', {1.5, 12, 1}, {0.5, 0.5, 0.5})
lightpanel:set_material(white)
lightpanel:set_illumination(white_light)
scene:add_child(lightpanel)

gr.render(scene, 'refraction.png', 512, 512,
	  {1, 5, 35}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
