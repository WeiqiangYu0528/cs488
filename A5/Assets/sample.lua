-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({0.23, 0.41, 0.53}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
brown = gr.material({0.58, 0.41, 0.32}, {0.5, 0.4, 0.8}, 25)
bowlColor = gr.material({0.46, 0.45, 0.44}, {0.5, 0.4, 0.8}, 25)
houseColor = gr.material({0.99, 0.60, 0.22}, {0.5, 0.4, 0.8}, 25)
houseBottomColor = gr.material({0.99, 0.74, 0.69}, {0.5, 0.4, 0.8}, 25)
houseColor1 = gr.material({0.23, 0.41, 0.53}, {0.5, 0.4, 0.8}, 25)
houseColor2 = gr.material({0.41, 0.50, 0.27}, {0.5, 0.4, 0.8}, 25)
houseColor3 = gr.material({0.56, 0.50, 0.91}, {0.5, 0.4, 0.8}, 25)
grass = gr.material({0.47, 0.64, 0.17}, {0.5, 0.4, 0.8}, 25)
pyramidColor = gr.material({0.93, 0.81, 0.39}, {0.5, 0.4, 0.8}, 25)
redColor =  gr.material({0.94, 0.25, 0.06},{0.5, 0.4, 0.8}, 25)
crystalColor = gr.material({0.42, 0.76, 0.68}, {0.5, 0.4, 0.8}, 25)
leaf = gr.material({0.13, 0.37, 0.09}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('root')
scene:rotate('x', 18)

plane = gr.mesh( 'plane', 'plane.obj' )
plane:set_material(grass)
plane:scale(90, 90, 90)

sphere = gr.sphere('sphere')
sphere:scale(1, 1, 1)
sphere:translate(6, 0.75, 11)
sphere:set_material(mat2)

cube = gr.cube('cube')
cube:scale(1.5, 1.5, 1.5)
cube:translate(7.25, 0, 12)
cube:set_material(mat2)

pyramid = gr.mesh('pyramid', 'pyramid.obj')
pyramid:rotate("x", -90)
pyramid:scale(11, 11, 11)
pyramid:translate(-6, 0, 10)
pyramid:set_material(pyramidColor)


cat = gr.mesh('cat', 'cat.obj')
cat:set_material(mat2)
cat:rotate("x", -90)
cat:rotate("y", 45)
cat:scale(0.6, 0.6, 0.6)
cat:translate(7.25, 6.5, 9)


house_bottom = gr.mesh("house_bottom", 'house_bottom.obj')
house_bottom:set_material(houseBottomColor)

door = gr.mesh("door", 'door.obj')
door:translate(0, -0.05, 0)
door:set_material(brown)

house = gr.mesh("house_top", 'house_top.obj')
house:scale(4.25, 4.25, 4.25)
house:translate(-8.5, 1.125, -30)
house:rotate("y", 25)
house:set_material(houseColor)
house:add_child(house_bottom)
house:add_child(door)

house1 = gr.mesh("house_top", 'house_top.obj')
house1:scale(4.25, 4.25, 4.25)
house1:translate(-1.5, 1.125, -30)
house1:rotate("y", 10)
house1:set_material(houseColor1)
house1:add_child(house_bottom)
house1:add_child(door)

house2 = gr.mesh("house_top", 'house_top.obj')
house2:scale(4.25, 4.25, 4.25)
house2:translate(2.5, 1.125, -30)
house2:rotate("y", -10)
house2:set_material(houseColor2)
house2:add_child(house_bottom)
house2:add_child(door)

house3 = gr.mesh("house_top", 'house_top.obj')
house3:scale(4.25, 4.25, 4.25)
house3:translate(8.5, 1.125, -30)
house3:rotate("y", -25)
house3:set_material(houseColor3)
house3:add_child(house_bottom)
house3:add_child(door)


column = gr.cube("column")
column:scale(1.5, 4.5, 1.5)
column:translate(-8.5, 0, 6)
column:set_material(mat2)

ball = gr.sphere("sphere", "sphere")
ball:scale(0.5, 0.2, 0.5)
ball:translate(0.5, 1.2, 0.5)
ball:set_material(mat2)
column:add_child(ball)

crystal = gr.mesh("crystal", 'crystals.obj')
crystal:set_material(mat3)
crystal:rotate("y", 150)
crystal:scale(0.5, 0.5, 0.5)
crystal:translate(-6, 0.25, 20)

crystal1 = gr.mesh("crystal", 'crystals.obj')
crystal1:set_material(crystalColor)
crystal1:rotate("y", -70)
crystal1:scale(0.5, 0.5, 0.5)
crystal1:translate(6, 0.25, 20)

crystal2 = gr.mesh("crystal", 'crystals.obj')
crystal2:set_material(redColor)
crystal2:rotate("y", 70)
crystal2:scale(0.4, 0.4, 0.4)
crystal2:translate(-2.5, 0.25, 13.5)

crystal3 = gr.mesh("crystal", 'crystals.obj')
crystal3:set_material(houseColor3)
crystal3:rotate("y", 0)
crystal3:scale(0.4, 0.4, 0.4)
crystal3:translate(2, 0.25, 13.5)

bark = gr.mesh("bark", 'bark.obj')
bark:set_material(brown)

tree = gr.mesh("tree", 'tree.obj')
tree:scale(1.25, 1.25, 1.25)
tree:translate(-12, 3, 8)
tree:set_material(leaf)
tree:add_child(bark)


scene:add_child(tree)
scene:add_child(pyramid)
scene:add_child(plane)
scene:add_child(sphere)
scene:add_child(column)
scene:add_child(cat)
scene:add_child(cube)

scene:add_child(house)
scene:add_child(house1)
scene:add_child(house2)
scene:add_child(house3)
scene:add_child(crystal)
scene:add_child(crystal1)
scene:add_child(crystal2)
scene:add_child(crystal3)

gr.render(scene,
   'sample.png', 512, 512,
   {0, 2, 40}, {0, 0, -1}, {0, 1, 0}, 50,
   {0.4, 0.4, 0.4}, {gr.light({200, 102, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})