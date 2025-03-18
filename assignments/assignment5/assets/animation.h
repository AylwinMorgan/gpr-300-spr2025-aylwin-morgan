#pragma once
#include <glm/ext.hpp>
#include <vector>

class Vec3Keyframe {
public:
	Vec3Keyframe() {
		time = 0;
	}
	Vec3Keyframe(float x, float y, float z, float t = 0.0f) {
		value[0] = x;
		value[1] = y;
		value[2] = z;
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
	~AnimationClip() {
		for (Vec3Keyframe* k : positionFrames) {
			delete k;
		}
		for (Vec3Keyframe* k : rotationFrames) {
			delete k;
		}
		for (Vec3Keyframe* k : scaleFrames) {
			delete k;
		}
	}
	float duration;
	std::vector<Vec3Keyframe*> positionFrames;
	std::vector<Vec3Keyframe*> rotationFrames;
	std::vector<Vec3Keyframe*> scaleFrames;
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