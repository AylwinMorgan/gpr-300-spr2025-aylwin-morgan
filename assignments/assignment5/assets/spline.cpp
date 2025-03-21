#include "spline.h"

glm::vec3 BezierCurve::lerp(glm::vec3 a, glm::vec3 b, float t) {
	glm::vec3 result = glm::vec3(0);
	result.x = (1.0 - t) * a.x + t * b.x;
	result.y = (1.0 - t) * a.y + t * b.y;
	result.z = (1.0 - t) * a.z + t * b.z;
	return result;
}

glm::vec3 BezierCurve::getPointFromKnot(Knot* k) {
	//glm::vec3 point = glm::vec3(0,0,k->scale);
	
	//point = glm::rotate();

	//glm::vec3 point = k->position + k->distanceToPoint * k->scale;
	
	glm::vec3 point = k->position + glm::vec3(1,1,1);

	return point;
}

glm::vec3 BezierCurve::getPointAtTime(float t) {
	glm::vec3 p0 = firstKnot->position;
	glm::vec3 p1 = getPointFromKnot(firstKnot);
	glm::vec3 p2 = getPointFromKnot(secondKnot);
	glm::vec3 p3 = secondKnot->position;
	glm::vec3 a = BezierCurve::lerp(p0,p1,t);
	glm::vec3 b = BezierCurve::lerp(p1,p2,t);
	glm::vec3 c = BezierCurve::lerp(p2,p3,t);
	glm::vec3 d = BezierCurve::lerp(a,b,t);
	glm::vec3 e = BezierCurve::lerp(b,c,t);
	return BezierCurve::lerp(d,e,t);
}