// Termm-Fall 2023

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  m_interaction_mode(0),
	  m_left_mouse_button_active(false),
	  m_right_mouse_button_active(false),
	  m_middle_mouse_button_active(false),
	  m_mouse_button_active(false),
	  m_mouse_GL_coordinate(1.0f),
	  m_prev_mouse_GL_coordinate(1.0f),
	  renderArcball(false),
	  zBuffer(true),
	  backfaceCulling(false),
	  frontfaceCulling(false),
	  m_translation(mat4()),
	  m_rotation(mat4()),
	  m_model(mat4())
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	initArcModel();

	initModelStack();

	initSceneNodeMapping();

	initMoveCommand();
	
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

void A3::initMoveCommand() {
	m_jointNodes.resize(JointNode::jointInstanceCount);
	for (auto& pair: m_nodeMap) {
		SceneNode * node = pair.second;
		if (node->m_nodeType == NodeType::JointNode) {
			JointNode* jointNode = dynamic_cast<JointNode *>(node);
			m_jointNodes[jointNode->m_jointId] = jointNode;
		}
	}

	m_command = std::make_unique<MoveCommand>(m_jointNodes);
	if (!m_command) {
		std::cerr << "Failed to create move command"<< std::endl;
	}

	m_jointAngles.resize(JointNode::jointInstanceCount);
	for (JointNode* jointNode: m_jointNodes) {
		m_jointAngles[jointNode->m_jointId] = std::make_pair(jointNode->x_angle, jointNode->y_angle);
	}

	m_command->init(m_jointAngles);
}

void A3::initModelStack() {
	while( !m_model_stack.empty() ) {
		m_model_stack.pop();
	}
	m_model_stack.push(glm::mat4());
}

void A3::initArcModel() {
	float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
	if( aspect > 1.0 ) {
		m_arc_model = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
	} else {
		m_arc_model = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
	}
	m_arc_model_invert = glm::inverse(m_arc_model);
}

void A3::initSceneNodeMapping() {
	SceneNode * root = m_rootNode.get();
	root->traverse(m_nodeMap);
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	// m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.25f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	if (m_interaction_mode == 0 && m_mouse_button_active) {
		updateModelMatrix();
	} 

	if (m_interaction_mode == 1 && m_mouse_button_active) {
		updateSceneNodeTransformations();
	}

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	// Add more gui elements here here ...
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Application")) {
			if (ImGui::MenuItem("Reset Position", "I")) {
				// Handle the "Open" action
				resetPosition();
			}
			if (ImGui::MenuItem("Reset Orientation", "O")) {
				// Handle the "Save" action
				resetOrientation();
			}
			if (ImGui::MenuItem("Reset Joints", "S")) {
				// Handle the "Save" action
				resetJoints();
			}
			if (ImGui::MenuItem("Reset All", "A")) {
				// Handle the "Save" action
				resetAll();
			}
			if (ImGui::MenuItem("Quit", "Q")) {
				// Handle the "Save" action
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "U")) {
				// Handle the "Open" action
				m_command->undo();
			}
			if (ImGui::MenuItem("Redo", "R")) {
				// Handle the "Save" action
				m_command->redo();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options")) {
			ImGui::Checkbox("Circle, C", &renderArcball);
			ImGui::Checkbox("Z-buffer, Z", &zBuffer);
			ImGui::Checkbox("Backface culling, B", &backfaceCulling);
			ImGui::Checkbox("Frontface culling, F", &frontfaceCulling);
			ImGui::EndMenu();
		}
		// Add more menus as needed
		ImGui::EndMainMenuBar();
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		ImGui::RadioButton( "Position/Orientation (P)", &m_interaction_mode, 0);
		ImGui::RadioButton( "Joints (J)",  &m_interaction_mode, 1);


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text( "x: %.5f, y: %.5f",  m_mouse_GL_coordinate.x,  m_mouse_GL_coordinate.y);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.isSelected ? glm::vec3( 0.4, 0.6, 0.74) : node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (zBuffer) {
		glEnable( GL_DEPTH_TEST );
	}
	if (backfaceCulling || frontfaceCulling) {
		glEnable( GL_CULL_FACE );
		if (backfaceCulling && frontfaceCulling) {
			glCullFace( GL_FRONT_AND_BACK );
		} else if (backfaceCulling) {
			glCullFace( GL_BACK );
		} else {
			glCullFace( GL_FRONT );
		}
	}
	
	glEnable(GL_STENCIL_TEST);
  	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	if (renderArcball) {
		renderArcCircle();
	}
	renderSceneGraph(m_rootNode.get());

	if (zBuffer) {
		glDisable( GL_DEPTH_TEST );
	}
	if (backfaceCulling || frontfaceCulling) {
		glDisable( GL_CULL_FACE );
	}

	glDisable( GL_STENCIL_TEST );
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode * root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	m_model = root->get_transform() * m_translation * m_rotation * glm::inverse(root->get_transform()); 
	traverseSceneGraph(root);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::traverseSceneGraph(const SceneNode *node) {
	m_model_stack.push(node->get_transform());
	m_model = m_model * m_model_stack.top();

	for (SceneNode * childNode : node->children) {
		traverseSceneGraph(childNode);
	}

	if (node->m_nodeType == NodeType::GeometryNode) {
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
		glStencilFunc(GL_ALWAYS, geometryNode->m_nodeId + 1, -1);
		updateShaderUniforms(m_shader, *geometryNode, m_view, m_model);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();

		// node->set_transform(prevModel);
	}

	m_model = m_model * glm::inverse(m_model_stack.top());
	m_model_stack.pop();
}
//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {

	glBindVertexArray(m_vao_arcCircle);
	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( m_arc_model ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	m_prev_mouse_GL_coordinate = m_mouse_GL_coordinate;
	m_mouse_GL_coordinate = vec2 (
			(2.0f * xPos) / m_windowWidth - 1.0f,
			1.0f - ( (2.0f * yPos) / m_windowHeight)
	);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (actions == GLFW_PRESS) {
		m_prev_mouse_GL_coordinate = m_mouse_GL_coordinate;
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_mouse_button_active = true;
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_left_mouse_button_active = true;
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				m_middle_mouse_button_active = true;
			} 
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_right_mouse_button_active = true;
			}

			if (m_interaction_mode == 1 && m_left_mouse_button_active) {
				double xpos, ypos;
				GLuint index;
				glfwGetCursorPos( m_window, &xpos, &ypos );
				GLint x = xpos * double(m_framebufferWidth) / double(m_windowWidth);
				GLint y = ypos * double(m_framebufferHeight) / double(m_windowHeight);
				y = m_framebufferHeight - y - 1;
				glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

				if ( index != 0 ) {
					bool isSelected = m_nodeMap[index - 1]->isSelected;
					m_nodeMap[index - 1]->isSelected = !isSelected;
					m_nodeMap[index - 2]->isSelected = !isSelected;
				}
			}
		}
	}

	if (actions == GLFW_RELEASE) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_mouse_button_active = false;
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_left_mouse_button_active = false;
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				m_middle_mouse_button_active = false;
			} 
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_right_mouse_button_active = false;
			}

			if (m_interaction_mode == 1 && (button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_RIGHT)) {
				m_command->execute(m_jointAngles);
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

void A3::updateModelMatrix() {
	glm::mat4 transM(1.0f);
	float offsetX = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	float offsetY = m_mouse_GL_coordinate.y - m_prev_mouse_GL_coordinate.y;
	if (m_left_mouse_button_active) {	
		transM[3][0] = offsetX;
		transM[3][1] = offsetY;
		m_translation =  m_translation * transM;
	}
	if (m_middle_mouse_button_active) {
		transM[3][2] = -offsetY;
		m_translation =  m_translation * transM;
	}
	if (m_right_mouse_button_active) {
		if (isMouseHoverOverArcCircle()) {
			float angle = 0.0f;
			glm::vec3 axis(0.0f);
			getAngleAndAxis(angle, axis);
			axis = glm::normalize(axis);
			transM = getRotationMatrix(angle, axis);
			// transM = glm::transpose(transM);
			// transM = vAxisRotMatrix(axis);
			// transM = glm::transpose(transM);
		} else {
			float cosine = cosf(offsetX);
			float sine = sinf(offsetX);
			transM[0][0] = cosine;
			transM[0][1] = sine;
			transM[1][0] = -sine;
			transM[1][1] = cosine;
		}
		m_rotation = m_rotation * transM;
	}
}

void A3::updateSceneNodeTransformations() {
	glm::mat4 transX(1.0f);
	glm::mat4 transY(1.0f);
	glm::mat4 transM(1.0f);
	float offsetX = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	float offsetY = m_mouse_GL_coordinate.y - m_prev_mouse_GL_coordinate.y;
	float rotatedRadiansX = glm::radians(offsetY * 100);
	float rotatedRadiansY = glm::radians(offsetX * 100);
	double rotatedAngleX = glm::degrees(rotatedRadiansX);
	double rotatedAngleY = glm::degrees(rotatedRadiansY);
	if (m_middle_mouse_button_active) {
		for (auto& nodePair: m_nodeMap) {
			SceneNode* node = nodePair.second;
			if (node->m_nodeType == NodeType::JointNode && node->isSelected) {
				JointNode* jointNode = static_cast<JointNode*>(node);
				double x_angle = jointNode->x_angle + rotatedAngleX;
				double y_angle = jointNode->y_angle + rotatedAngleY;
				if (jointNode->m_joint_x.min <= x_angle && 
					jointNode->m_joint_x.max >= x_angle) {
					m_jointAngles[jointNode->m_jointId].first = x_angle;
					transX = glm::rotate(transX, rotatedRadiansX, glm::vec3(1.0f, 0.0f, 0.0f));
					jointNode->x_angle = x_angle;
					jointNode->set_transform(jointNode->get_transform() * transX);
				}
				if (jointNode->m_joint_y.min <= y_angle && 
					jointNode->m_joint_y.max >= y_angle) {
					m_jointAngles[jointNode->m_jointId].second = y_angle;
					transY = glm::rotate(transY, rotatedRadiansY, glm::vec3(0.0f, 1.0f, 0.0f));
					jointNode->y_angle = y_angle;
					jointNode->set_transform(jointNode->get_transform() * transY);
				}
			}
		}
	}
	
	if (m_right_mouse_button_active) {
		SceneNode* node = m_nodeMap[32];
		if (node->isSelected) {
			JointNode* jointNode = static_cast<JointNode*>(node);
			double y_angle = jointNode->y_angle + rotatedAngleY;
			if (jointNode->m_joint_y.min <= y_angle && 
				jointNode->m_joint_y.max >= y_angle) {
				m_jointAngles[jointNode->m_jointId].second = y_angle;
				transM = glm::rotate(transM, rotatedRadiansY, glm::vec3(0.0f, 1.0f, 0.0f));
				jointNode->y_angle = y_angle;
				jointNode->set_transform(jointNode->get_transform() * transM);
			}
		}
	}
}

void A3::resetPosition() {
	m_translation = glm::mat4();
}

void A3::resetOrientation() {
	m_rotation = glm::mat4();
}

void A3::resetJoints() {
	m_command->reset();
}

void A3::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints();
}

bool A3::isMouseHoverOverArcCircle() {
	glm::vec2 mousePos(m_arc_model_invert * glm::vec4(m_mouse_GL_coordinate, 0.0f, 1.0f));
	if (mousePos.x * mousePos.x + mousePos.y * mousePos.y <= 1.0f) {
		return true;
	}
	return false;
}

glm::vec3 A3::getTrackballVector(glm::vec2& mousePos) {
	glm::vec3 trackballPos(m_arc_model_invert * glm::vec4(mousePos, 0.0f, 1.0f));
	float z_square = 1 - (trackballPos.x * trackballPos.x + trackballPos.y * trackballPos.y);
	if (z_square >= 0) {
		trackballPos.z = sqrt(z_square);
	} else {
		trackballPos = glm::normalize(trackballPos); 
	}
	return trackballPos;
}

void A3::getAngleAndAxis(float& angle, glm::vec3& axis) {
	glm::vec3 prev = getTrackballVector(m_prev_mouse_GL_coordinate);
	glm::vec3 cur = getTrackballVector(m_mouse_GL_coordinate);
	angle = acos(std::min(1.0f, glm::dot(prev, cur)));
	axis = glm::cross(prev, cur);
}

glm::mat4 A3::getRotationMatrix(float& angle, glm::vec3& axis) {
	// chose the x-axis
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	glm::mat4 RyPhi1(1.0f);
	glm::mat4 RzPsi1(1.0f);
	glm::mat4 RyPhi2(1.0f);
	glm::mat4 RzPsi2(1.0f);
	glm::mat4 Rx(1.0f);

	float cosine = cosf(angle);
	float sine = sinf(angle);

	float cosPhi = x / sqrt(x * x + z * z);
	float sinPhi = z / sqrt(x * x + z * z);
	float cosPsi = sqrt(x * x + z * z);
	float sinPsi = y;

	RyPhi1[0][0] = cosPhi;
	RyPhi1[0][2] = -sinPhi;
	RyPhi1[2][0] = sinPhi;
	RyPhi1[2][2] = cosPhi;

	RyPhi2[0][0] = cosPhi;
	RyPhi2[0][2] = sinPhi;
	RyPhi2[2][0] = -sinPhi;
	RyPhi2[2][2] = cosPhi;

	RzPsi1[0][0] = cosPsi;
	RzPsi1[0][1] = -sinPsi;
	RzPsi1[1][0] = sinPsi;
	RzPsi1[1][1] = cosPsi;

	RzPsi2[0][0] = cosPsi;
	RzPsi2[0][1] = sinPsi;
	RzPsi2[1][0] = -sinPsi;
	RzPsi2[1][1] = cosPsi;

	Rx[1][1] = cosine;
	Rx[1][2] = sine;
	Rx[2][1] = -sine;
	Rx[2][2] = cosine;

	return RyPhi2 * RzPsi2 * Rx * RzPsi1 * RyPhi1;
}



/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
// glm::mat4 A3::vAxisRotMatrix(glm::vec3& axis) {
//     float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
// 	float fVecX = axis.x; 
// 	float fVecY = axis.y;
// 	float fVecZ = axis.z;

// 	glm::mat4 mNewMat(1.0f);

//     /* Find the length of the vector which is the angle of rotation
//      * (in radians)
//      */
//     fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

//     /* If the vector has zero length - return the identity matrix */
//     if (fRadians > -0.000001 && fRadians < 0.000001) {
//         return mNewMat;
//     }

//     /* Normalize the rotation vector now in preparation for making
//      * rotation matrix. 
//      */
//     // fInvLength = 1 / fRadians;
//     // fNewVecX   = fVecX * fInvLength;
//     // fNewVecY   = fVecY * fInvLength;
//     // fNewVecZ   = fVecZ * fInvLength;
// 	axis = glm::normalize(axis);
// 	fNewVecX = axis.x;
//     fNewVecY = axis.y;
//     fNewVecZ = axis.z;

//     /* Create the arbitrary axis rotation matrix */
//     double dSinAlpha = sin(fRadians);
//     double dCosAlpha = cos(fRadians);
//     double dT = 1 - dCosAlpha;

//     mNewMat[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
//     mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
//     mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
//     mNewMat[0][3] = 0;

//     mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
//     mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
//     mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
//     mNewMat[1][3] = 0;

//     mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
//     mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
//     mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
//     mNewMat[2][3] = 0;

//     mNewMat[3][0] = 0;
//     mNewMat[3][1] = 0;
//     mNewMat[3][2] = 0;
//     mNewMat[3][3] = 1;

// 	return mNewMat;
// }


MoveCommand::MoveCommand(std::vector<JointNode *>& jointNodes)
: jointNodes(jointNodes)
{
	// empty
}

void MoveCommand::init(std::vector<std::pair<double, double>>& jointAngles)  
{
	jointAngleList.push_back(jointAngles);
	curJointAngle = jointAngleList.begin();
}

void MoveCommand::execute(std::vector<std::pair<double, double>>& jointAngles)
{
	std::next(curJointAngle) = jointAngleList.end();
	jointAngleList.push_back(jointAngles);
	curJointAngle++;
}

void MoveCommand::redo() {
	// if (curJointAngle + 1 != jointAngleList.end()) {

	// }

}

void MoveCommand::undo() {
	cout << "call undo" << endl;
	if (curJointAngle != jointAngleList.begin()) {
		glm::mat4 transX(1.0f);
		glm::mat4 transY(1.0f);
		auto& jointAngles = *curJointAngle;
		auto& prevJointAngles = *(std::prev(curJointAngle));
		cout << jointAngles.size() << endl;
		cout << prevJointAngles.size() << endl;
		for (size_t i = 0; i < jointAngles.size(); ++i) {
			double x_angle = prevJointAngles[i].first - jointAngles[i].first;
			double y_angle = prevJointAngles[i].second - jointAngles[i].second;
			float rotatedRadiansX = glm::radians(x_angle);
			float rotatedRadiansY = glm::radians(y_angle);
			transX = glm::rotate(transX, rotatedRadiansX, glm::vec3(1.0f, 0.0f, 0.0f));
			cout << "before joint node";
			jointNodes[i]->x_angle = prevJointAngles[i].first;
			jointNodes[i]->set_transform(jointNodes[i]->get_transform() * transX);
			cout << "after joint node";
			transY = glm::rotate(transY, rotatedRadiansY, glm::vec3(0.0f, 1.0f, 0.0f));
			jointNodes[i]->y_angle = prevJointAngles[i].second;
			jointNodes[i]->set_transform(jointNodes[i]->get_transform() * transY);
		}
	} else {
		// pop up error
	}
}

void MoveCommand::reset() {
}



