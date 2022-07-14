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
	bool enableRootMotion = false;

public:
	Animator(Skeleton* skeleton, AnimationClip& animClip, double startTime);
	void ChangeAnimationClip(AnimationClip& animClip, double startTime);
	void SetSkeletonPose(AnimationPose& pose);
	double GetCurrentClipTime();
	const AnimationClip& GetCurrentClip();
	AnimationPose GetPoseAtCurrentTime();
	void SetRootMotionEnable(bool enable);

	
	void Update(double deltaTime);
};

