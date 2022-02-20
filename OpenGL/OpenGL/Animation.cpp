#include "Animation.h"

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, float framePerSecond, int frameCount, bool loop)
	:clipName(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount), loop(loop)
{
}

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, float framePerSecond, int frameCount, bool loop,
	std::vector<AnimationPoseSample> samples)
	: clipName(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount),
		loop(loop),samples(samples)
{
}

void AnimationClip::AddNewSample(AnimationPoseSample sample)
{
	this->samples.push_back(sample);
}
