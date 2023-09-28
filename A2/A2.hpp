// Termm--Fall 2023

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};

class Cube {
public:
	Cube();
	void initCube();

	std::vector<std::pair<glm::vec4, glm::vec4>> lines;
};

enum class MouseButton {
	None,
	LEFT,
	MIDDLE,
	RIGHT
};

enum class Mode {
	RotateView,
	TranslateView,
	Perspective,
	RotateModel,
	TranslateModel,
	ScaleModel,
	Viewport
};

class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	void reset();
	void drawCube(Cube& cube);
	glm::vec2 projection(glm::vec4& position);
	void translate(glm::vec4& position, bool view);
	void scale(glm::vec4& position);
	void rotate(glm::vec4& position, bool view);
	void transform(glm::vec4& position);


	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object


	VertexData m_vertexData;

	// 2-component vector of floats.
	// Represents mouse position in GL coordinates.
	// Coordinates span [-1,1] along x and y directions within window.
	glm::vec2 m_mouse_GL_coordinate;
	glm::vec2 m_prev_mouse_GL_coordinate;
	// True if mouse button is currently down.
	bool m_mouseButtonActive;
	MouseButton m_mouseButton;

	Mode m_mode;
	int m_mode_index;


	glm::vec3 m_currentLineColour;

};
