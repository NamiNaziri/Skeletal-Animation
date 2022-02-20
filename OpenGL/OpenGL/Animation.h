#pragma once
#include "EngineMath.h"
#include "Mesh.h"
#include "vector"

struct AnimationPoseSample
{
	std::vector<EngineMath::SQT> jointPoses;
};


class AnimationClip
{
private:
	std::string clipName;
	Skeleton* skeleton; //TODO pointer has to be replaced with skeletonUnique Id
	float framePerSecond;
	int frameCount;

	std::vector<AnimationPoseSample> samples;
	bool loop;


public:
	AnimationClip(std::string clipName, Skeleton* skeleton, float framePerSecond, int frameCount, bool loop);
	AnimationClip(std::string clipName, Skeleton* skeleton, float framePerSecond, int frameCount, bool loop, std::vector<AnimationPoseSample> samples);
	void AddNewSample(AnimationPoseSample sample);
	
};