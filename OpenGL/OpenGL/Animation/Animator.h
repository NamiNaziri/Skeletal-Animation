#pragma once
#include <string>

#include "Animation.h"
#include "../SkeletalModel.h"
class Animator
{
private:
	double startTimeForCurrentAnim;
	double currentTime;
	std::string currentClipName;
	Skeleton* skeleton;
	AnimationClip* currentClip;
	bool loop = true;


public:
	Animator(Skeleton* skeleton, AnimationClip& animClip, double startTime);
	void ChangeAnimationClip(AnimationClip& animClip, double startTime);
	void Update(double deltaTime);
};

