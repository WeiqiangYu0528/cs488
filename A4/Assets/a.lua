-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({0.23, 0.41, 0.53}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
deskColor = gr.material({0.58, 0.41, 0.32}, {0.5, 0.4, 0.8}, 25)
bowlColor = gr.material({0.46, 0.45, 0.44}, {0.5, 0.4, 0.8}, 25)
houseColor = gr.material({0.99, 0.60, 0.22}, {0.5, 0.4, 0.8}, 25)
houseBottomColor = gr.material({0.99, 0.74, 0.69}, {0.5, 0.4, 0.8}, 25)
houseColor1 = gr.material({0.23, 0.41, 0.53}, {0.5, 0.4, 0.8}, 25)
houseColor2 = gr.material({0.41, 0.50, 0.27}, {0.5, 0.4, 0.8}, 25)
houseColor3 = gr.material({0.56, 0.50, 0.91}, {0.5, 0.4, 0.8}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
pyramidColor = gr.material({0.93, 0.81, 0.39}, {0.0, 0.0, 0.0}, 0)

scene = gr.node('root')
cat = gr.mesh('cat', 'cat.obj')
scene:add_child(cat)

gr.render(scene,
	  'sample.png', 512, 512,
	  {0, 10, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})