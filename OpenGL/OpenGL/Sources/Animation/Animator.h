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
	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<AnimationClip> currentClip;
	bool loop = true;
	bool enableRootMotion = false;

public:
	Animator(std::shared_ptr<Skeleton> skeleton, std::shared_ptr<AnimationClip> animClip, double startTime);
	void ChangeAnimationClip(std::shared_ptr<AnimationClip> animClip, double startTime);
	void SetSkeletonPose(AnimationPose& pose);
	double GetCurrentClipTime();
	const AnimationClip& GetCurrentClip();
	AnimationPose GetPoseAtCurrentTime();
	void SetRootMotionEnable(bool enable);

	
	void Update(double deltaTime);
};

