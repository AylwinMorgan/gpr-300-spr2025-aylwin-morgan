#include "../assets/joint.h"

Joint::Joint() {
	localPosition = glm::vec3(0, 0, 0);
	localRotation = glm::vec3(0, 0, 0);
	localScale = glm::vec3(1, 1, 1);
	parent = nullptr;
}

Joint::Joint(std::string partName) {
	name = partName;
	localPosition = glm::vec3(0,0,0);
	localRotation = glm::vec3(0,0,0);
	localScale = glm::vec3(1,1,1);
	parent = nullptr;
}

Joint* Joint::addChild(std::string partName) {
	Joint* child = new Joint(partName);
	children.push_back(child);
	child->parent = this;
	return child;
}

glm::mat4 Joint::getGlobalTransform() {
	Joint* current = parent;
	glm::mat4 transform = glm::mat4(1.0f);
	while (current != nullptr) {
		transform = glm::translate(transform, current->localPosition);
		transform = glm::rotate(transform, current->localRotation.x, glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, current->localRotation.y, glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, current->localRotation.z, glm::vec3(0, 0, 1));
		transform = glm::scale(transform, current->localScale);
		current = current->parent;
	}
	globalTransform = transform;
	return transform;
}