
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
darkRed = gr.material({0.1, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.46, 0.55, 0.75}, {0.1, 0.1, 0.1}, 10)

-- Create the top level root node named 'root'.
rootNode = gr.node('rootNode')
rootNode:translate(0.0, 0.0, -6.0)

function drawTorso()
    torso = gr.mesh('sphere', 'my_torso')
    torso:scale(1.7, 1.4, 1.0)
	torso:translate(0.0, -0.2, 0.0)
    torso:set_material(white)
	rootNode:add_child(torso)
    return torso
end

function drawShoulder()
    shoulderJoint = gr.joint('shoulderJoint', {-90, 0, 90}, {-90, 0, 90})
    shoulderJoint:translate(0.0, 0.5, 0.0)
	rootNode:add_child(shoulderJoint)
end


function drawHip()
    hipJoint = gr.joint('hipJoint', {-90, 0, 90}, {-90, 0, 90})
    hipJoint:translate(0.0, -1.0, 0.0)
	rootNode:add_child(hipJoint)
end

function drawArms()
    leftArmJoint = gr.joint('leftUpperArmJoint', {-90, 0, 90}, {-90, 0, 90})
    leftArm = gr.mesh('sphere', 'my_left_upper_arm')

    rightArmJoint = gr.joint('rightUpperArmJoint', {-90, 0, 90}, {-90, 0, 90})
    rightArm = gr.mesh('sphere', 'my_right_upper_arm')

    leftArmJoint:translate(-1.2, 0.0, 0.0)
    rightArmJoint:translate(1.2, 0.0, 0.0)
    
    -- leftUpperArm:translate(0.0, -1.0, 0.0)
    -- rightUpperArm:translate(0.0, -1.0, 0.0)
    leftArm:set_material(blue)
    rightArm:set_material(blue)

    leftArm:scale(1.4, 0.4, 0.35)
    rightArm:scale(1.4, 0.4, 0.35)

    shoulderJoint:add_child(leftArmJoint)
    shoulderJoint:add_child(rightArmJoint)

    leftArmJoint:add_child(leftArm)
    rightArmJoint:add_child(rightArm)
end

-- function drawForearm()
--     leftForearmJoint = gr.joint('leftForearmJoint', {-90, 0, 90}, {-90, 0, 90})
--     leftForearm = gr.mesh('sphere', 'my_left_forearm')

--     rightForearmJoint = gr.joint('rightForearmJoint', {-90, 0, 90}, {-90, 0, 90})
--     rightForearm = gr.mesh('sphere', 'my_right_forearm')

--     leftForearmJoint:translate(0.0, -1.0, 0.0)
--     rightForearmJoint:translate(0.0, -1.0, 0.0)

--     leftForearm:translate(0.0, -1.0, 0.0)
--     rightForearm:translate(0.0, -1.0, 0.0)
--     leftForearm:set_material(red)
--     rightForearm:set_material(red)

--     leftForearm:scale(0.06, 0.5, 0.03)
--     rightForearm:scale(0.06, 0.5, 0.03)

--     leftUpperArmJoint:add_child(leftForearmJoint)
--     rightUpperArmJoint:add_child(rightForearmJoint)

--     leftForearmJoint:add_child(leftForearm)
--     rightForearmJoint:add_child(rightForearm)
-- end

-- function drawHand(left)
--     leftHandJoint = gr.joint('leftHandJoint', {-90, 0, 90}, {-90, 0, 90})
--     leftHand = gr.mesh('sphere', 'my_left_hand')

--     rightHandJoint = gr.joint('rightHandJoint', {-90, 0, 90}, {-90, 0, 90})
--     rightHand = gr.mesh('sphere', 'my_right_hand')

--     leftHandJoint:translate(0.0, -1.0, 0.0)
--     rightHandJoint:translate(0.0, -1.0, 0.0)

--     leftHand:scale(0.1, 0.1, 0.1)
--     rightHand:scale(0.1, 0.1, 0.1)
--     leftHand:set_material(green)
--     rightHand:set_material(green)

--     leftForearmJoint:add_child(leftHandJoint)
--     rightForearmJoint:add_child(rightHandJoint)

--     leftHandJoint:add_child(leftHand)
--     rightHandJoint:add_child(rightHand)
-- end

-- function drawThigh(left)
--     leftThighJoint = gr.joint('leftThighJoint', {-90, 0, 90}, {-90, 0, 90})
--     leftThigh = gr.mesh('sphere', 'my_left_thigh')

--     rightThighJoint = gr.joint('rightThighJoint', {-90, 0, 90}, {-90, 0, 90})
--     rightThigh = gr.mesh('sphere', 'my_right_thigh')

--     leftThighJoint:translate(-0.7, 0.0, 0.0)
--     rightThighJoint:translate(0.7, 0.0, 0.0)    

--     leftThigh:scale(0.1, 0.5, 0.1)
--     rightThigh:scale(0.1, 0.5, 0.1)
--     leftThigh:set_material(blue)
--     rightThigh:set_material(blue)

--     leftThigh:translate(0.0, -0.5, 0.0)
--     rightThigh:translate(0.0, -0.5, 0.0)

--     hipJoint:add_child(leftThighJoint)
--     hipJoint:add_child(rightThighJoint)

--     leftThighJoint:add_child(leftThigh)
--     rightThighJoint:add_child(rightThigh)
-- end

-- function drawCalf(left)
--     leftCalfJoint = gr.joint('leftCalfJoint', {-90, 0, 90}, {-90, 0, 90})
--     leftCalf = gr.mesh('sphere', 'my_left_calf')

--     rightCalfJoint = gr.joint('rightCalfJoint', {-90, 0, 90}, {-90, 0, 90})
--     rightCalf = gr.mesh('sphere', 'my_right_calf')

--     leftCalfJoint:translate(0.0, -1.0, 0.0)
--     rightCalfJoint:translate(0.0, -1.0, 0.0)

--     leftCalf:scale(0.1, 0.4, 0.1)
--     rightCalf:scale(0.1, 0.4, 0.1)

--     leftCalf:translate(0.0, -0.4, 0.0)
--     rightCalf:translate(0.0, -0.4, 0.0)
--     leftCalf:set_material(red)
--     rightCalf:set_material(red)

--     leftThighJoint:add_child(leftCalfJoint)
--     rightThighJoint:add_child(rightCalfJoint)

--     leftCalfJoint:add_child(leftCalf)
--     rightCalfJoint:add_child(rightCalf)
-- end

function drawFoot()
    leftFootJoint = gr.joint('leftFootJoint', {-90, 0, 90}, {-90, 0, 90})
    leftFoot = gr.mesh('sphere', 'my_left_foot')

    rightFootJoint = gr.joint('rightFootJoint', {-90, 0, 90}, {-90, 0, 90})
    rightFoot = gr.mesh('sphere', 'my_right_foot')

    leftFootJoint:translate(-1.0, -0.7, -0.5)
    rightFootJoint:translate(1.0, -0.7, -0.5)

    leftFoot:scale(0.5, 0.35, 0.50)
    rightFoot:scale(0.5, 0.35, 0.50)
    leftFoot:set_material(blue)
    rightFoot:set_material(blue)

    hipJoint:add_child(leftFootJoint)
    hipJoint:add_child(rightFootJoint)

    leftFootJoint:add_child(leftFoot)
    rightFootJoint:add_child(rightFoot)
end

-- function drawNeck()
    -- neckJoint = gr.joint('neckJoint', {-90, 0, 90}, {-90, 0, 90})
    -- neckJoint:translate(0.0, 1.4, 0.0)

--     neck = gr.mesh('sphere', 'my_neck')
--     neck:scale(0.25, 0.3, 0.25)
--     neck:set_material(red)

--     neckJoint:add_child(neck)
--     rootNode:add_child(neckJoint)
-- end

function drawHead()
    headJoint = gr.joint('headJoint', {-90, 0, 90}, {-90, 0, 90})
    headJoint:translate(0.0, 1.5, 0.0)

    head= gr.mesh('sphere', 'my_head')
    head:scale(1.1, 0.9, 0.5)
    head:set_material(white)

    headJoint:add_child(head)
    -- neckJoint:add_child(headJoint)
	-- rootNode:add_child(neckJoint)
	rootNode:add_child(headJoint)
end

function drawEar()
	left_ear = gr.mesh('sphere', 'my_left_ear')
	right_ear = gr.mesh("sphere", "my_right_ear")
    left_ear:scale(0.3, 0.6, 0.3)
	left_ear:translate(-0.25, 0.75, 0.0)
	left_ear:rotate("z", 30)
    left_ear:set_material(blue)

	right_ear:scale(0.3, 0.6, 0.3)
	right_ear:translate(0.25, 0.75, 0.0)
	right_ear:rotate("z", -30)
    right_ear:set_material(blue)

    headJoint:add_child(left_ear)
	headJoint:add_child(right_ear)
    -- neckJoint:add_child(headJoint)
	-- rootNode:add_child(neckJoint)

end


-- function drawEyes()

--     leftEye = gr.mesh('sphere', 'my_left_eye')
--     rightEye = gr.mesh('sphere', 'my_right_eye')

--     leftEye:scale(0.1, 0.1, 0.1)
--     leftEye:translate(-0.2, 0.0, 0.5)
--     leftEye:set_material(black)

--     rightEye:scale(0.1, 0.1, 0.1)
--     rightEye:translate(0.2, 0.0, 0.5)
--     rightEye:set_material(black)

--     headJoint:add_child(leftEye)
--     headJoint:add_child(rightEye)
-- end

drawTorso()
drawShoulder()
drawArms()
drawHip()
-- rootNode:add_child(createHip())
-- drawUpperArms()
-- drawForearm()
-- drawHand()
-- drawThigh()
-- drawCalf()
drawFoot()
-- drawNeck()
drawHead()
drawEar()
-- drawEyes()

return rootNode
