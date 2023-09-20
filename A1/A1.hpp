// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();
	void initMaze();
	void initSphere();
	void initLight();
	void reset();
	void dig();
	void drawCube(float* verts, int i, int j, size_t& ct);

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	ShaderProgram m_light_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.
	GLint light_uni; // Uniform location for light position.
	GLint light_col_uni; // Uniform location for light colour.
	GLint view_uni; // Uniform location for view position.

	GLint P_light_uni; // Uniform location for Projection matrix.
	GLint V_light_uni; // Uniform location for View matrix.
	GLint M_light_uni; // Uniform location for Model matrix.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to maze geometry.
	GLuint m_maze_vao; // Vertex Array Object
	GLuint m_maze_vbo; // Vertex Buffer Object

	// Fields related to sphere geometry.
	GLuint m_sphere_vao; // Vertex Array Object
	GLuint m_sphere_vbo; // Vertex Buffer Object
	GLuint m_sphere_ebo; // Element Buffer Object

	// Fields related to light source.
	GLuint m_light_vao; // Vertex Array Object
	GLuint m_light_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float floor_colour[3];
	float sphere_colour[3];
	float maze_colour[3];
	int current_col;

	Maze* m;
};
