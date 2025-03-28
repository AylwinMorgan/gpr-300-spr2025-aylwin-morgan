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

Joint::Joint(std::string partName, glm::vec3 lPosition, glm::vec3 lRotation, glm::vec3 lScale) {
	name = partName;
	localPosition = lPosition;
	localRotation = lRotation;
	localScale = lScale;
	parent = nullptr;
}

// will recursively clean up the skeleton
Joint::~Joint() {
	for (Joint* child : children) {
		delete child;
	}
}

// add child with default values
Joint* Joint::addChild(std::string partName) {
	Joint* child = new Joint(partName);
	children.push_back(child);
	child->parent = this;
	return child;
}

Joint* Joint::addChild(std::string partName, glm::vec3 lPosition, glm::vec3 lRotation, glm::vec3 lScale) {
	Joint* child = new Joint(partName, lPosition, lRotation, lScale);
	children.push_back(child);
	child->parent = this;
	return child;
}

glm::mat4 Joint::getGlobalTransform() {
	glm::mat4 transform = glm::mat4(1.0f);
	Joint* rootJoint = this;
	// all joints that affect this joint's transform (includes this joint)
	std::vector<Joint*> ancestorJoints;
	while (rootJoint != nullptr) {
		ancestorJoints.push_back(rootJoint);
		rootJoint = rootJoint->parent;
	}
	// goes from the root node to the current node to calculate transformation matrix
	for (int i = ancestorJoints.size() - 1; i >= 0; i--) {
		transform = glm::translate(transform, ancestorJoints[i]->localPosition);
		transform = glm::rotate(transform, glm::radians(ancestorJoints[i]->localRotation.x), glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, glm::radians(ancestorJoints[i]->localRotation.y), glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, glm::radians(ancestorJoints[i]->localRotation.z), glm::vec3(0, 0, 1));
		transform = glm::scale(transform, ancestorJoints[i]->localScale);

	}
	globalTransform = transform;
	return transform;
}