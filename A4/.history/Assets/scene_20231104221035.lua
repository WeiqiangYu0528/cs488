-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
deskColor = gr.material({0.58, 0.41, 0.32}, {0.5, 0.4, 0.8}, 25)
bowlColor = gr.material({0.46, 0.45, 0.44}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

bowl = gr.mesh( 'bowl', 'bowl.obj' )
bowl:scale(30, 30, 30)
bowl:translate(0, 7, 0)
bowl:set_material(bowlColor)
scene_root:add_child(bowl)

desk = gr.mesh( 'desk', 'DeskDrawer.obj' )
desk:scale(10, 10, 10)
desk:set_material(deskColor)
desk:add_child(bowl)
scene_root:add_child(desk)


gr.render(scene_root,
	  'desk.png', 512, 512,
	  {0, 10, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})