// Termm-Fall 2023

#pragma once

#include <glm/glm.hpp>
#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
	double x_angle, y_angle;
	unsigned int m_jointId;
	glm::mat4 initTrans;

	// The number of SceneNode instances.
	static unsigned int jointInstanceCount;
};
