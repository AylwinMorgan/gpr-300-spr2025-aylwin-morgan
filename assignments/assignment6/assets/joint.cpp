#include "../assets/joint.h"

Joint::Joint() {

}

Joint::Joint(std::string partName) {
	name = partName;
}

Joint* Joint::addChild(std::string partName) {
	Joint* child = new Joint(partName);
	children.push_back(child);
	return child;
}

glm::mat4 Joint::getGlobalTransform() {
	Joint* current = parent;
	glm::mat4 transform = localTransform;
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