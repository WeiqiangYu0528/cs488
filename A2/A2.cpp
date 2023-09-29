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

const float PI = 3.14159265f;

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
	  m_mouse_GL_coordinate(vec2(0.0)),
	  m_prev_mouse_GL_coordinate(vec2(0.0)),
	  m_mode(Mode::TranslateModel),
	  m_mode_index(4),
	  m_modelM(mat4(1.0f)),
	  m_viewM(mat4(1.0f)),
	  m_camera(Camera())
{
	m_frustum = createFrustum(1.0f, 100.0f);
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

	Cube cube;
	Gnomon modelGnomon(true);

	drawCube(cube);
	// drawGnomon(modelGnomon);
	// drawGnomon(gnomon)
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
	m_mouse_GL_coordinate = vec2 (
			(2.0f * xPos) / m_windowWidth - 1.0f,
			1.0f - ( (2.0f * yPos) / m_windowHeight)
	);

	if (m_mouseButtonActive) {
		// m_shape_translation = m_mouse_GL_coordinate;
		transform();
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
			m_prev_mouse_GL_coordinate = m_mouse_GL_coordinate;
			// m_shape_translation = m_mouse_GL_coordinate;
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_mouseButton = MouseButton::LEFT;
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				m_mouseButton = MouseButton::MIDDLE;
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_mouseButton = MouseButton::RIGHT;
			}
		}
	}

	if (actions == GLFW_RELEASE) {
		m_mouseButtonActive = false;
		m_mouseButton = MouseButton::None;
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
	}
	return eventHandled;
}

void A2::reset () {
	m_modelM = mat4(1.0f);
	m_viewM = mat4(1.0f);
	// Fill in with event handling code...
}

void A2::drawCube(Cube& cube) {
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	for (auto& edge: cube.edges){
		glm::vec3 v0(m_modelM * edge.first);
		glm::vec3 v1(m_modelM * edge.second);

		if (m_frustum.isInsideFrustum(v0, v1)) {
			drawLine(projection(v0), projection(v1));
		}
	}
}

void A2::drawGnomon(Gnomon& gnomon) {
	// for (size_t i = 0; i < gnomon.edges.size(); ++i) {
	// 	auto& edge = gnomon.edges[i];
	// 	glm::vec4& v0 = edge.first;
	// 	glm::vec4& v1 = edge.second;
	// 	if (m_mode != Mode::ScaleModel) {
	// 		// transform(v0);
	// 		// transform(v1);
	// 	}
	// 	if (clipNearPlane(v0, v1)) {
	// 		setLineColour(gnomon.colours[i]);
	// 		drawLine(projection(v0), projection(v1));
	// 	}
	// }
}

void A2::transform() {
	switch (m_mode) {
		case Mode::TranslateModel:
			translate();
			break;
		case Mode::ScaleModel:
			scale();
			break;
		case Mode::RotateModel:
			rotate();
			break;
		// case Mode::TranslateView:
		// 	translate(position, true);
		// 	break;
		// case Mode::RotateView:
		// 	rotate(position, true);
		// 	break;
		default:
			break;
	}
}

glm::vec2 A2::projection(glm::vec3& position) {

	// Fill in with event handling code...
	return glm::vec2(position.x / position.z, position.y / position.z);
}

void A2::translate() {
	glm::mat4 transM(1.0f);
	float offset = (m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x) * 0.05;
	if (m_mouseButton == MouseButton::LEFT) {		
    	transM[3] = glm::vec4(offset, 0.0f, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		transM[3] = glm::vec4(0.0f, offset, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		transM[3] = glm::vec4(0.0f, 0.0f, offset, 1.0f);
	}
	// if (view) {
	// 	transM = glm::inverse(transM);
	// }
	m_modelM = transM * m_modelM;
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
	// modelM = modelM * scaleM;
	// position = modelM * position;
	m_modelM = m_modelM * scaleM;
}

void A2::rotate() {
	glm::mat4 rotateM(1.0f);
	float offset = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	float cosine = std::cos(offset);
	float sine = std::sin(offset);
	if (m_mouseButton == MouseButton::LEFT) {
		// Fill in with event handling code...
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
	// if (view) {
	// 	rotateM = glm::inverse(rotateM);
	// }
	// modelM = modelM * rotateM;
	// position = modelM * position;
	m_modelM = m_modelM * rotateM;
}

// // near plane: (-1, -1, 1) -> (1, 1, 1)
// bool A2::clipNearPlane(glm::vec4& v1, glm::vec4& v2) {
// 	glm::vec4 norm(0, 0, 1, 1);
// 	glm::vec4 p(0, 0, 1, 1);
// 	return clipLine(v1, v2, p, norm);
// }

Frustum A2::createFrustum(float near, float far)
{
    Frustum frustum;
	m_framebufferWidth = 2048;
	m_framebufferHeight = 1536;
	float aspect = float( m_framebufferWidth ) / float( m_framebufferHeight );
    float halfFarHeight = far * tanf(0.5 * fov);
    float halfFarWidth = halfFarHeight * aspect;
	std::cout << m_framebufferWidth << m_framebufferHeight <<  aspect << " " << halfFarWidth << " " << halfFarHeight << std::endl;
    glm::vec3 farCenter = far * m_camera.cameraFront;

    frustum.planes[0] = Plane(m_camera.cameraPos + near * m_camera.cameraFront, m_camera.cameraFront);
    frustum.planes[1] = Plane(m_camera.cameraPos + farCenter, -m_camera.cameraFront);
    frustum.planes[2] = Plane(m_camera.cameraPos, glm::cross(farCenter - m_camera.cameraRight * halfFarWidth, m_camera.cameraUp));
    frustum.planes[3] = Plane(m_camera.cameraPos, glm::cross(m_camera.cameraUp, farCenter + m_camera.cameraRight * halfFarWidth));
    frustum.planes[4] = Plane(m_camera.cameraPos, glm::cross(m_camera.cameraRight, farCenter - m_camera.cameraUp * halfFarHeight));
    frustum.planes[5] = Plane(m_camera.cameraPos, glm::cross(farCenter + m_camera.cameraUp * halfFarHeight, m_camera.cameraRight));

    return frustum;
}

Cube::Cube() {
	initCube();
}

void Cube::initCube() {
	vertices = {
		vec4(-0.5, -0.5, -1.5, 1),
		vec4(-0.5, 0.5, -1.5, 1),
		vec4(0.5, 0.5, -1.5, 1),
		vec4(0.5, -0.5, -1.5, 1),
		vec4(-0.5, -0.5, -2.5, 1),
		vec4(-0.5, 0.5, -2.5, 1),
		vec4(0.5, 0.5, -2.5, 1),
		vec4(0.5, -0.5, -2.5, 1),
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


Gnomon::Gnomon(bool model) : model(model)
{
	initGnomon();
}

void Gnomon::initGnomon() {
	edges = {
		{vec4(0.0f, 0.0f, 2.0f, 1.0f), vec4(0.125f, 0.0f, 2.0f, 1.0f)},
		{vec4(0.0f, 0.0f, 2.0f, 1.0f), vec4(0.0f, 0.125f, 2.0f, 1.0f)},
		{vec4(0.0f, 0.0f, 2.0f, 1.0f), vec4(0.0f, 0.0f, 2.125f, 1.0f)}
	};
	
	colours = {
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f) 
	};
}

Camera::Camera() {
	initCamera();
}

void Camera::initCamera() {
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraRight = glm::normalize(glm::cross(cameraFront, up));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

Plane::Plane(glm::vec3 p, glm::vec3 norm) : 
	point(p), norm(glm::normalize(norm))
{

}

bool Plane::clipLine(glm::vec3& p1, glm::vec3& p2) {
	int wecA = glm::dot((p1 - point), norm);
	int wecB = glm::dot((p2 - point), norm);
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

bool Frustum::isInsideFrustum(glm::vec3& p1, glm::vec3& p2) {
	int i = 0;
	for (Plane& plane: planes) {
		if (!plane.clipLine(p1, p2)) {
			// std::cout << i << std::endl;
			return false;
		}
		++i;
	}
	return true;
}
