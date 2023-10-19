// Termm-Fall 2023

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "GeometryNode.hpp"
#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <stack>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class Command
{
public:
  virtual ~Command() {}
  virtual void execute() = 0;
};

class MoveCommand : public Command
{
public:
  MoveCommand();
  virtual ~MoveCommand() = default;
  virtual void init(std::vector<std::pair<double, double>> jointAngles);
  virtual void execute();
  virtual void redo();
  virtual void undo();

private:
//   std::list<std::vector<std::pair<double, double>>> jointAngleList;
//   std::list<std::vector<std::pair<double, double>>>::iterator curJointAngle;
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode *node);
	void renderArcCircle();
	void traverseSceneGraph(const SceneNode *node);
	void updateModelMatrix();
	void updateSceneNodeTransformations();
	void initModelStack();
	void initArcModel();
	void initSceneNodeMapping();
	bool isMouseHoverOverArcCircle();
	void getAngleAndAxis(float& angle, glm::vec3& axis);
	glm::vec3 getTrackballVector(glm::vec2& mousePos);
	glm::mat4 getRotationMatrix(float& angle, glm::vec3& axis);

	void resetPosition();
	void resetOrientation();
	void resetJoints();
	void resetAll();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;
	glm::mat4 m_model;
	glm::mat4 m_arc_model;
	glm::mat4 m_arc_model_invert;
	glm::mat4 m_translation;
	glm::mat4 m_rotation;
	std::stack<glm::mat4> m_model_stack;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	int m_interaction_mode;
	bool m_mouse_button_active;
	bool m_left_mouse_button_active;
	bool m_middle_mouse_button_active;
	bool m_right_mouse_button_active;
	bool renderArcball;
	bool zBuffer;
	bool backfaceCulling;
	bool frontfaceCulling;

	glm::vec2 m_mouse_GL_coordinate;
	glm::vec2 m_prev_mouse_GL_coordinate;

	std::shared_ptr<MoveCommand> m_command;
	std::map<int, SceneNode *> m_nodeMap;
	// std::unordered_map<int, std::vector<SceneNode *>> m_jointMap;
	// glm::mat4 vAxisRotMatrix(glm::vec3& axis);
};
