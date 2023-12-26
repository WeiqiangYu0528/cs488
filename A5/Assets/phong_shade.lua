white = gr.material({0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25)
red = gr.material({1, 1, 1}, {0.45, 0.15, 0.2}, {0.25, 0.25, 0.25}, 25)
blue = gr.material({1, 1, 1}, {0.15, 0.15, 0.5}, {0.25, 0.25, 0.25}, 25)
grey= gr.material({1, 1, 1}, {0.5, 0.5, 0.5}, {0.25, 0.25, 0.25}, 25)
mushroom_mat = gr.material( {0.25, 0.25, 0.25}, {0, 0, 0}, {0.25, 0.25, 0.25}, 25)
brown = gr.material({0.58, 0.41, 0.32}, {0.58, 0.41, 0.32}, {0.5, 0.4, 0.8}, 25)
houseBottomColor = gr.material({0.99, 0.74, 0.69}, {0.99, 0.74, 0.69}, {0.5, 0.4, 0.8}, 25)
houseColor = gr.material({0.23, 0.41, 0.53}, {0.23, 0.41, 0.53}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('scene')
 
house = gr.node("house")
house:scale(2.5, 2.5, 2.5)
house:translate(0, -8, 4)
scene:add_child(house)

mushroom_top = gr.mesh('mushroom_top', 'Mushroom_top.obj')
mushroom_top:set_material(houseColor)
house:add_child(mushroom_top)

mushroom_door = gr.mesh('mushroom_door', 'Mushroom_door.obj')
mushroom_door:set_material(brown)
house:add_child(mushroom_door)

mushroom_bottom = gr.mesh('mushroom_bottom', 'Mushroom_bottom.obj')
mushroom_bottom:set_material(houseBottomColor)
house:add_child(mushroom_bottom)

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

white_light = gr.light({1.5, 8, 25}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 2, 2, 2, {1, 1, 1}, {1, 0, 0})
lightpanel = gr.nh_rect('lightpanel', {1.5, 8, 25}, {2, 2, 2})
lightpanel:set_material(white)
lightpanel:set_illumination(white_light)
scene:add_child(lightpanel)


gr.render(scene, 'phong_shade.png', 512, 512,
	  {1, 3, 35}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
