

mat1  = gr.material({0.7, 0.6, 1.0}, {0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)	
mat2  = gr.material({1.0, 0.6, 0.1}, {1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 250)

white = gr.material({0.73, 0.73, 0.73}, {0.73, 0.73, 0.73}, {0.1, 0.1, 0.1}, 25)
red = gr.material({0.65, 0.05, 0.05}, {0.65, 0.05, 0.05}, {0.1, 0.1, 0.1}, 25)
green = gr.material({0.12, 0.45, 0.15}, {0.12, 0.45, 0.15}, {0.1, 0.1, 0.1}, 25)


mirror = gr.material( {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25)

blue = gr.material({0.1386, 0.1327, 0.45}, {0.1386, 0.1327, 0.45}, {0.1, 0.05, 0.5}, 25)
grey= gr.material({0.4649, 0.4823, 0.5002}, {0.4649, 0.4823, 0.5002}, {0.3, 0.3, 0.3}, 25)

scene = gr.node('root')

-- reflective_sphere = gr.sphere('sphere1')
-- reflective_sphere:scale(4, 4, 4)
-- reflective_sphere:translate(-6.5, 2.5, 0)
-- reflective_sphere:set_material(mirror)
-- reflective_sphere:set_reflection(1.0, 1)
-- scene:add_child(reflective_sphere)

    
floor = gr.plane('floor')
floor:scale(300, 1, 300)
floor:translate(-50, 0, -50)
floor:set_material(grey)
scene:add_child(floor)

backwall = gr.plane('backwall')
backwall:rotate('x', 90)
backwall:scale(300, 300, 1)
backwall:translate(-50, 50, -20)
backwall:set_material(grey)
scene:add_child(backwall)

leftwall = gr.plane("leftwall")
leftwall:rotate('z', -90)
leftwall:scale(1, 300, 300)
leftwall:translate(-17, 100, -30)
leftwall:set_material(green)
scene:add_child(leftwall)

rightwall = gr.plane('rightwall')
rightwall:rotate('z', 90)
rightwall:scale(1, 300, 300)
rightwall:translate(17, -100, -30)
rightwall:set_material(red)
scene:add_child(rightwall)

ceiling = gr.plane('ceiling')
ceiling:rotate('x', 180)
ceiling:scale(300, 1, 300)
ceiling:translate(-50, 25, 50)
ceiling:set_material(grey)
scene:add_child(ceiling)

-- frontwall = gr.plane('backwall')
-- frontwall:rotate('x', -90)
-- frontwall:scale(100, 100, 1)
-- frontwall:translate(-15, -5, 50)
-- frontwall:set_material(grey)
-- scene:add_child(frontwall)


white_light = gr.light({-2, 25.0, -10}, {1, 0, 0}, {0, 0, 1}, 6, 6, {256, 256, 256}, {1, 0, 0})

lightpanel = gr.nh_plane('lightpanel', {-2, 25.0, -10}, {0, -1, 0}, 6)
lightpanel:set_material(white)
lightpanel:set_illumination(white_light)
scene:add_child(lightpanel)
--     -- // Light
--     -- world.add(make_shared<quad>(point3(213,554,227), vec3(130,0,0), vec3(0,0,105), light));

--     -- // Box 1
--     -- shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
--     -- box1 = make_shared<rotate_y>(box1, 15);
--     -- box1 = make_shared<translate>(box1, vec3(265,0,295));
--     -- world.add(box1);

--     -- // Box 2
--     -- shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
--     -- box2 = make_shared<rotate_y>(box2, -18);
--     -- box2 = make_shared<translate>(box2, vec3(130,0,65));
--     -- world.add(box2);


gr.render(scene, 'test2.png', 512, 512,
    {0, 11, 40}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {white_light})

