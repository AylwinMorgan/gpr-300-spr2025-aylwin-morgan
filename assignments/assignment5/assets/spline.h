#pragma once
#include <glm/ext.hpp>

struct Knot {
	glm::vec3 angles = glm::vec3(0, 0, 0);
	float scale = 1.0f;
	glm::vec3 position = glm::vec3(0, 0, 0);
};

class BezierCurve {
public:
	~BezierCurve() {
		delete firstKnot;
		delete secondKnot;
	}
	Knot* firstKnot;
	Knot* secondKnot;
	BezierCurve() {
		firstKnot = new Knot;
		secondKnot = new Knot;
	}
	glm::vec3 getPointFromKnot(Knot* k);
	static glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
	void setPoint(int point, glm::vec3 value);
	glm::vec3 getPointAtTime(float t);
};
