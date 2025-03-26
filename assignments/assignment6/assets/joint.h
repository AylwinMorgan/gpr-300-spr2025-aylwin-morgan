#pragma once
#include <glm/ext.hpp>
#include <string>
#include <vector>

class Joint {
public:
	glm::mat4 globalTransform;
	glm::mat4 localTransform;
	glm::vec3 localPosition;
	glm::vec3 localRotation;
	glm::vec3 localScale;
	Joint* parent;
	std::vector<Joint*> children;
	std::string name;

	Joint();
	Joint(std::string partName);
	Joint* addChild(std::string partName);
	glm::mat4 getGlobalTransform();
};