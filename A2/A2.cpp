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
	  m_mode_index(4)
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

	drawCube(cube);


	// Draw inner square:
	// setLineColour(vec3(0.2f, 1.0f, 1.0f));
	// drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	// drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	// drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	// drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));
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
	// Fill in with event handling code...
}

void A2::drawCube(Cube& cube) {
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	for (auto& line: cube.lines){
		glm::vec4& v0 = line.first;
		glm::vec4& v1 = line.second;

		transform(v0);
		transform(v1);

		drawLine(projection(v0), projection(v1));
	}
}

void A2::transform(glm::vec4& position) {
	switch (m_mode) {
		case Mode::TranslateModel:
			translate(position, false);
			break;
		case Mode::ScaleModel:
			scale(position);
			break;
		case Mode::RotateModel:
			rotate(position, false);
			break;
		case Mode::TranslateView:
			translate(position, true);
			break;
		case Mode::RotateView:
			rotate(position, true);
			break;
		default:
			break;
	}
}

glm::vec2 A2::projection(glm::vec4& position) {

	// Fill in with event handling code...
	//orthographic projection
	glm::mat4x3 orthographicMatrix = glm::mat4x3(
        1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f
    );
	return glm::vec2(orthographicMatrix * position);
}

void A2::translate(glm::vec4& position, bool view) {
	glm::mat4 transM = glm::mat4(1.0f);
	float offset = m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x;
	if (m_mouseButton == MouseButton::LEFT) {
		// Fill in with event handling code...
		
    	transM[3] = glm::vec4(offset, 0.0f, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		transM[3] = glm::vec4(0.0f, offset, 0.0f, 1.0f);
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		transM[3] = glm::vec4(0.0f, 0.0f, offset, 1.0f);
	}
	if (view) {
		transM = glm::inverse(transM);
	}
	position = transM * position;
}

void A2::scale(glm::vec4& position) {
	glm::mat4 scaleM = glm::mat4(1.0f);
	float offset = std::max(0.0f, 1.0f + m_mouse_GL_coordinate.x - m_prev_mouse_GL_coordinate.x);
	if (m_mouseButton == MouseButton::LEFT) {
		// Fill in with event handling code...
		
    	scaleM[0][0] = offset;
	}
	else if (m_mouseButton == MouseButton::MIDDLE) {
		scaleM[1][1] = offset;
	}
	else if (m_mouseButton == MouseButton::RIGHT) {
		scaleM[2][2] = offset;
	}
	position = scaleM * position;
}

void A2::rotate(glm::vec4& position, bool view) {
	glm::mat4 rotateM = glm::mat4(1.0f);
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
	if (view) {
		rotateM = glm::inverse(rotateM);
	}
	position = rotateM * position;
}

// void A2::viewRotate() {
// 	glm::mat4 rotateM = glm::mat4(1.0f);
// }

// void A2::viewTranslate() {
// 	glm::mat4 rotateM = glm::mat4(1.0f);

// }


Cube::Cube() {
	initCube();
}

void Cube::initCube() {
	lines = {
		{vec4(-0.5f, -0.5f, -0.5f, 1.0f), vec4(0.5f, -0.5f, -0.5f, 1.0f)},
		{vec4(-0.5f, 0.5f, -0.5f, 1.0f), vec4(0.5f, 0.5f, -0.5f, 1.0f)},
		{vec4(-0.5f, -0.5f, 0.5f, 1.0f), vec4(0.5f, -0.5f, 0.5f, 1.0f)},
		{vec4(-0.5f, 0.5f, 0.5f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1.0f)},
		{vec4(-0.5f, -0.5f, -0.5f, 1.0f), vec4(-0.5f, -0.5f, 0.5f, 1.0f)},
		{vec4(-0.5f, 0.5f, -0.5f, 1.0f), vec4(-0.5f, 0.5f, 0.5f, 1.0f)},
		{vec4(0.5f, -0.5f, -0.5f, 1.0f), vec4(0.5f, -0.5f, 0.5f, 1.0f)},
		{vec4(0.5f, 0.5f, -0.5f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1.0f)}
	};

}
