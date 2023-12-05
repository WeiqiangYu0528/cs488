white = gr.material({0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25)
red = gr.material({1, 1, 1}, {0.45, 0.15, 0.2}, {0.25, 0.25, 0.25}, 25)
blue = gr.material({1, 1, 1}, {0.15, 0.15, 0.5}, {0.25, 0.25, 0.25}, 25)
grey= gr.material({1, 1, 1}, {0.5, 0.5, 0.5}, {0.25, 0.25, 0.25}, 25)


green = gr.material({1, 1, 1}, {0.47, 0.64, 0.17}, {0.5, 0.4, 0.8}, 25)
yellow = gr.material({1, 1, 1}, {0.93, 0.81, 0.39}, {0.5, 0.4, 0.8}, 25)


scene = gr.node('scene')

rect = gr.nh_rect("rect", {1, 1, 1}, {2, 3, 2})
rect:set_material(green)
scene:add_child(rect)

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

white_light = gr.light({1.5, 6, 20}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 2, 1, 2, {1, 1, 1}, {1, 0, 0})

light_panel = gr.nh_rect('light_panel', {1.5, 6, 20}, {2, 1, 2})
light_panel:set_material(white)
light_panel:set_illumination(white_light)
scene:add_child(light_panel)

gr.render(scene, 'test4.png', 512, 512,
	  {1, 3, 35}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
