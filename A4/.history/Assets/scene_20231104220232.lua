-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

bowl = gr.mesh( 'bowl', 'bowl.obj' )
bowl:scale(100, 100, 100)
bowl:set_material(mat3)
scene_root:add_child(bowl)

desk = gr.mesh( 'desk', 'DeskDrawer.obj' )
desk:scale(10, 10, 10)
desk:set_material(mat3)
desk:add_child(bowl)
scene_root:add_child(desk)


gr.render(scene_root,
	  'desk.png', 512, 512,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})