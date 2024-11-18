// Termm--Fall 2024

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	void reset_joint_x();
	void reset_joint_y();

	glm::mat4 getJointRotationMatrix();

	struct JointRange {
		double min, init, max;
	};

	double currXRot, currYRot;

	JointRange m_joint_x, m_joint_y;
};
