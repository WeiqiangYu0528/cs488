// Termm--Fall 2023

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f)),
	  m_mouseButton(MouseButton::None),
	  m_mouseButtonActive(false),
	  m_leftMouseButtonActive(false),
	  m_mouse_GL_coordinate(vec2(0.0)),
	  m_prev_mouse_GL_coordinate(vec2(0.0)),
	  m_mode(Mode::RotateModel),
	  m_mode_index(3),
	  m_camera(Camera()),
	  m_cube(Cube()),
	  m_modelGnomon(Gnomon(true)),
	  m_worldGnomon(Gnomon(false)),
	  m_viewport_start_coordinate(-0.9f, 0.9f),
	  m_viewport_end_coordinate(0.9f, -0.9f)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	m_camera.initViewMatrix(m_viewM);

	initProjectionMatrix();

	m_frustum = createFrustum();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2022
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	drawViewport();
	drawCube();
	drawGnomon(m_modelGnomon);
	drawGnomon(m_worldGnomon);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		ImGui::RadioButton( "Rotate View (o)", &m_mode_index, 0);
		ImGui::RadioButton( "Translate View (e)",  &m_mode_index, 1);
		ImGui::RadioButton( "Perspective (p)",  &m_mode_index, 2);
		ImGui::RadioButton( "Rotate Model (r)",  &m_mode_index, 3);
		ImGui::RadioButton( "Translate Model (t)", &m_mode_index, 4);
		ImGui::RadioButton( "Scale Model (s)",  &m_mode_index, 5);
		ImGui::RadioButton( "Viewport (v)",  &m_mode_index, 6);

		m_mode = static_cast<Mode>(m_mode_index);

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset Application" ) ) {
			reset();
		}

		ImGui::Text( "fov: %.1f, near: %.1f, far: %.1f", fov, near, far);
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
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

	if (m_mouseButtonActive) {
		transform();
		if (m_leftMouseButtonActive) {
			m_viewport_end_coordinate.x = glm::clamp(m_mouse_GL_coordinate.x, -0.995f, 0.995f);
			m_viewport_end_coordinate.y = glm::clamp(m_mouse_GL_coordinate.y, -0.995f, 0.995f);
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (actions == GLFW_PRESS) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_mouseButtonActive = true;
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_mouseButton = MouseButton::LEFT;
				if (m_mode == Mode::Viewport) {
					m_leftMouseButtonActive = true;
					m_viewport_start_coordinate = m_mouse_GL_coordinate;
					m_viewport_end_coordinate = m_mouse_GL_coordinate;
				}
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				m_mouseButton = MouseButton::MIDDLE;
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_mouseButton = MouseButton::RIGHT;
			}
		}
	}

	if (actions == GLFW_RELEASE) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_mouseButtonActive = false;
			m_mouseButton = MouseButton::None;
			if (m_leftMouseButtonActive) {
				m_leftMouseButtonActive = false;
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_A) {
			reset();
		}
		if (key == GLFW_KEY_O) {
			m_mode = Mode::RotateView;
			m_mode_index = 0;
		}
		if (key == GLFW_KEY_E) {
			m_mode = Mode::TranslateView;
			m_mode_index = 1;
		}
		if (key == GLFW_KEY_P) {
			m_mode = Mode::Perspective;
			m_mode_index = 2;
		}
		if (key == GLFW_KEY_R) {
			m_mode = Mode::RotateModel;
			m_mode_index = 3;
		}
		if (key == GLFW_KEY_T) {
			m_mode = Mode::TranslateModel;
			m_mode_index = 4;
		}
		if (key == GLFW_KEY_S) {
			m_mode = Mode::ScaleModel;
			m_mode_index = 5;
		}
		if (key == GLFW_KEY_V) {
			m_mode = Mode::Viewport;
			m_mode_index = 6;
		}
	}
	return true;
}

void A2::reset() {
	fov = 30.0f;
	near = 1.0f;
	far = 100.0f;
	m_cube.initCube();
	m_worldGnomon.initGnomon();
	m_modelGnomon.initGnomon();
	m_camera.initCamera();
	m_camera.initViewMatrix(m_viewM);
	m_frustum = createFrustum();
	m_mode = Mode::RotateModel;
	m_mode_index = 3;
	m_viewport_start_coordinate = glm::vec2(-0.9f, 0.9f);
	m_viewport_end_coordinate = glm::vec2(0.9f, -0.9f);
	m_mouse_GL_coordinate = glm::vec2(0.0f);
	m_prev_mouse_GL_coordinate = glm::vec2(0.0f);
	m_currentLineColour = glm::vec3(0.0f);
	m_mouseButtonActive = false;
	m_leftMouseButtonActive = false;
	m_mouseButton = MouseButton::None;
	initProjectionMatrix();
}

glm::vec4 A2::getProjectedPosition(glm::vec4& position, glm::mat4& modelM) {
	return m_projM * (m_viewM * (modelM * position));
}

void::A2::drawViewport() {
	setLineColour(vec3(0.0f, 0.0f, 0.0f));
	glm::vec2 upLeft(std::min(m_viewport_start_coordinate.x, m_viewport_end_coordinate.x), std::max(m_viewport_start_coordinate.y, m_viewport_end_coordinate.y));
	glm::vec2 bottomRight(std::max(m_viewport_start_coordinate.x, m_viewport_end_coordinate.x), std::min(m_viewport_start_coordinate.y, m_viewport_end_coordinate.y));
	glm::vec2 upRight(bottomRight.x, upLeft.y);
	glm::vec2 bottomLeft(upLeft.x, bottomRight.y);
	drawLine(upLeft, upRight);
	drawLine(upLeft, bottomLeft);
	drawLine(upRight, bottomRight);
	drawLine(bottomLeft, bottomRight);
}

void A2::drawCube() {
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	for (auto& edge: m_cube.edges){
		glm::vec4 v0(getProjectedPosition(edge.first, m_cube.modelM));
		glm::vec4 v1(getProjectedPosition(edge.second, m_cube.modelM));
		if (m_frustum.isInsideFrustum(v0, v1)) {
			homogenize(v0);
			homogenize(v1);
			drawLine(viewportTransform(v0), viewportTransform(v1));
		}
	}
}

void A2::drawGnomon(Gnomon& gnomon) {
	for (size_t i = 0; i < gnomon.edges.size(); ++i) {
		auto& edge = gnomon.edges[i];
		glm::vec4 v0(getProjectedPosition(edge.first, gnomon.modelM));
		glm::vec4 v1(getProjectedPosition(edge.second, gnomon.modelM));
		if (m_frustum.isInsideFrustum(v0, v1)) {
			setLineColour(gnomon.colours[i]);
			homogenize(v0);
			homogenize(v1);
			drawLine(viewportTransform(v0), viewportTransform(v1));
		}
	}
}

void A2::transform() {
	switch (m_mode) {
		case Mode::TranslateModel:
			translate(false);
			break;
		case Mode::ScaleModel:
			scale();
			break;
		case Mode::RotateModel:
			rotate(false);
			break;
		case Mode::TranslateView:
			translate(true);
			break;
		case Mode::RotateView:
			rotate(true);
			break;
		case Mode::Perspective:
			perspective();
		default:
			break;
	}
}

glm::vec2 A2::viewportTransform(glm::vec4& position) {
	glm::vec2 point(position);
	float viewportWidth = std::abs(m_viewport_end_coordinate.x - m_viewport_start_coordinate.x);
	float viewportHeight = std::abs(m_viewport_end_coordinate.y - m_viewport_start_coordinate.y);
	float viewportX = std::min(m_viewport_start_coordinate.x, m_viewport_end_coordinate.x);
	float viewportY = std::min(m_viewport_start_coordinate.y, m_viewport_end_coordinate.y);
	point.x =  (viewportWidth / 2.0f) * (point.x + 1.0f) + viewportX;
	point.y =  (viewportHeight / 2.0f) * (point.y + 1.0f) + viewportY;
	return point;
}

void A2::initProjectionMatrix() {
    m_projM = glm::mat4(0.0f);
	float aspect = 1.0f;
	float theta = glm::radians(fov);
	float cotValue = cosf(theta / 2) / sinf(theta / 2);
	m_projM[0][0] = cotValue / aspect;
	m_projM[1][1] = cotValue;
	m_projM[2][2] = (far + near) / (far - near);
	m_projM[2][3] = 1;
	m_projM[3][2] = -2 * far * near / (far - near);
}

void A2::homogenize(glm::vec4& position) {
	position.x /= position.w;
	position.y /= position.w;
	position.z /= position.w;
	position.w /= position.w;
}

void A2::translate(bool view) {
	glm::mat4 transM(1.0f);
	float offset = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	if (m_mouseButton == MouseButton::LEFT) {		
    	transM[3] = glm::vec4(offset, 0.0f, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		transM[3] = glm::vec4(0.0f, offset, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		transM[3] = glm::vec4(0.0f, 0.0f, offset, 1.0f);
	}
	if (view) {
		m_camera.updateCamera(transM);
		transM = glm::inverse(transM);
		m_viewM = transM * m_viewM;
	} else {
		m_cube.modelM = m_cube.modelM * transM;
		m_modelGnomon.modelM = m_modelGnomon.modelM * transM;
	}
}

void A2::scale() {
	glm::mat4 scaleM(1.0f);
	float offset = std::max(0.0f, 1.0f + m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x);
	if (m_mouseButton == MouseButton::LEFT) {
    	scaleM[0][0] = offset;
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		scaleM[1][1] = offset;
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		scaleM[2][2] = offset;
	}
	m_cube.modelM = m_cube.modelM * scaleM;
}

void A2::rotate(bool view) {
	glm::mat4 rotateM(1.0f);
	float offset = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	float cosine = cosf(offset);
	float sine = sinf(offset);
	if (m_mouseButton == MouseButton::LEFT) {
    	rotateM[1][1] = cosine;
		rotateM[1][2] = sine;
		rotateM[2][1] = -sine;
		rotateM[2][2] = cosine;
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		rotateM[0][0] = cosine;
		rotateM[0][2] = -sine;
		rotateM[2][0] = sine;
		rotateM[2][2] = cosine;
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		rotateM[0][0] = cosine;
		rotateM[0][1] = sine;
		rotateM[1][0] = -sine;
		rotateM[1][1] = cosine;
	}
	if (view) {
		m_camera.updateCamera(rotateM);
		rotateM = glm::inverse(rotateM);
		m_viewM = rotateM * m_viewM;
	} else {
		m_cube.modelM = m_cube.modelM * rotateM;
		m_modelGnomon.modelM = m_modelGnomon.modelM * rotateM;
	}
}

void A2::perspective() {
	float offset = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	if (m_mouseButton == MouseButton::LEFT) {
		fov = glm::clamp(fov + offset * 50, 5.0f, 160.0f);
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		near = glm::clamp(near + offset * 10, 0.0f, far);
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		far = glm::clamp(far + offset * 10, near, 100.0f);
	}
	initProjectionMatrix();
}

Frustum A2::createFrustum()
{
    Frustum frustum;

	frustum.viewLines[0] = glm::vec4(0, 0, 1, 1);
	frustum.viewLines[1] = glm::vec4(0, 0, -1, 1);
	frustum.viewLines[2] = glm::vec4(1, 0, 0, 1);
	frustum.viewLines[3] = glm::vec4(-1, 0, 0, 1);
	frustum.viewLines[4] = glm::vec4(0, -1, 0, 1);
	frustum.viewLines[5] = glm::vec4(0, 1, 0, 1);

    return frustum;
}

Cube::Cube() {
	initCube();
}

void Cube::initCube() {
	modelM = mat4(1.0f);
	vertices = {
		vec4(-1.0f, -1.0f, 1.0f, 1.0f),
		vec4(-1.0f, 1.0f, 1.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(1.0f, -1.0f, 1.0f, 1.0f),
		vec4(-1.0f, -1.0f, -1.0f, 1.0f),
		vec4(-1.0f, 1.0f, -1.0f, 1.0f),
		vec4(1.0f, 1.0f, -1.0f, 1.0f),
		vec4(1.0f, -1.0f, -1.0f, 1.0f),
	};

	edges = {
		{vertices[0], vertices[1]},
		{vertices[1], vertices[2]},
		{vertices[2], vertices[3]},
		{vertices[3], vertices[0]},
		{vertices[4], vertices[5]},
		{vertices[5], vertices[6]},
		{vertices[6], vertices[7]},
		{vertices[7], vertices[4]},
		{vertices[0], vertices[4]},
		{vertices[1], vertices[5]},
		{vertices[2], vertices[6]},
		{vertices[3], vertices[7]},
	};
}

Gnomon::Gnomon(bool model) : model(model) {
	initGnomon();
}

void Gnomon::initGnomon() {
	modelM = mat4(1.0f);
	edges = {
		{vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.25f, 0.0f, 0.0f, 1.0f)},
		{vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.25f, 0.0f, 1.0f)},
		{vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.25f, 1.0f)}
	};
	
	if (model) {
		colours = {vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)};
	} else {
		colours = {vec3(0.2f, 0.6f, 1.0f), vec3(1.0f, 0.4f, 0.4f), vec3(1.0f, 0.8f, 0.0f)};
	}
}

Camera::Camera() : up(0.0f, 1.0f, 0.0f) {
	initCamera();
}

void Camera::initCamera() {
	glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
	cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
	cameraFront = glm::normalize(cameraTarget - cameraPos);
	cameraRight = glm::normalize(glm::cross(cameraFront, up));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::initViewMatrix(glm::mat4& viewM) {
	glm::mat4 orientation = {
		vec4(cameraRight.x, cameraUp.x, cameraFront.x, 0.0f),
		vec4(cameraRight.y, cameraUp.y, cameraFront.y, 0.0f),
		vec4(cameraRight.z, cameraUp.z, cameraFront.z, 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
	};
	glm::mat4 translation = {
		vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(-cameraPos.x, -cameraPos.y, -cameraPos.z, 1.0f)
	};
	viewM = orientation * translation;
}

void Camera::updateCamera(glm::mat4& trans) {
	glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
	cameraPos = glm::vec3(trans * glm::vec4(cameraPos, 1.0f));
	cameraFront = glm::normalize(cameraTarget - cameraPos);
	cameraRight = glm::normalize(glm::cross(cameraFront, up));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

bool Frustum::isInsideFrustum(glm::vec4& p1, glm::vec4& p2) {
	for (auto& line: viewLines) {
		if (!clipLine(line, p1, p2)) {
			return false;
		}
	}
	return true;
}

bool Frustum::clipLine(glm::vec4& line, glm::vec4& p1, glm::vec4& p2) {
	float wecA = glm::dot(p1, line);
	float wecB = glm::dot(p2, line);
	if (wecA < 0 && wecB < 0) {
		return false;
	}
	if ( wecA >= 0 && wecB >= 0 ) {
		return true;
	}
	float t = wecA / (wecA - wecB);
	if ( wecA < 0 ) {
		p1 = p1 + t * (p2 - p1);
	} else {
		p2 = p1 + t * (p2 - p1);
	}
	return true;
}
