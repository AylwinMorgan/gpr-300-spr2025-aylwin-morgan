#pragma once
#include <glm/ext.hpp>
#include <vector>

class Vec3Keyframe {
public:
	Vec3Keyframe(float t) {
		time = t;
	}
	float time;
	float value[3] = { 0, 0, 0 };
};

class AnimationClip {
public:
	AnimationClip(float d = 4.0f) {
		duration = d;
	}
	float duration;
	std::vector<Vec3Keyframe> positionFrames;
	std::vector<Vec3Keyframe> rotationFrames;
	std::vector<Vec3Keyframe> scaleFrames;
};

class Animator {
public:
	Animator(AnimationClip* c) {
		clip = c;
	}
	~Animator() {
		delete clip;
	}
	AnimationClip* clip;
	bool isPlaying = false;
	float playbackTime = 0.0f;
	float playbackSpeed = 1.0f;
	bool isLooping = false;
};