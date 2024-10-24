// Termm-Fall 2024
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "cs488-framework/MathUtils.hpp"

#include "JointNode.hpp"
#include "helpers.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name),
	  currXRot(0),
	  currYRot(0)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::reset_joint_x()
{
	currXRot = m_joint_x.init;
}
void JointNode::reset_joint_y()
{
	currYRot = m_joint_y.init;
}

//---------------------------------------------------------------------------------------
double JointNode::get_clamped_addition_x(double degrees)
{
	return clampValue(currXRot + degrees, m_joint_x.max, m_joint_x.min) - currXRot;
}
//---------------------------------------------------------------------------------------
double JointNode::get_clamped_addition_y(double degrees)
{
	return clampValue(currYRot + degrees, m_joint_y.max, m_joint_y.min) - currYRot;
}

//---------------------------------------------------------------------------------------
void JointNode::update_joint_x(double degrees)
{
	currXRot = clampValue(currXRot + degrees, m_joint_x.max, m_joint_x.min);
}

//---------------------------------------------------------------------------------------
void JointNode::update_joint_y(double degrees)
{
	currYRot = clampValue(currYRot + degrees, m_joint_y.max, m_joint_y.min);
}

//---------------------------------------------------------------------------------------
glm::mat4 JointNode::getJointRotationMatrix()
{
	// rotate x then rotate y
	glm::mat4 rotation_matrix = glm::rotate((float)degreesToRadians(currXRot), glm::vec3(1.0f, 0.0, 0.0));
	rotation_matrix = glm::rotate((float)degreesToRadians(currYRot), glm::vec3(0.0f, 1.0, 0.0)) * rotation_matrix;

	return rotation_matrix;
}
