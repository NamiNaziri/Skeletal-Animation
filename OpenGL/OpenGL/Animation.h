#pragma once
#include "EngineMath.h"
#include "vector"
#include "SkeletalModel.h"
#include <map>

struct PositionKeyframe
{
	double time;
	glm::vec3 position;
	PositionKeyframe(double time, glm::vec3 position) : time(time), position(position) {}
};

struct RotationKeyframe
{
	double time;
	glm::quat rotation;
	RotationKeyframe(double time, glm::quat rotation) : time(time), rotation(rotation) {}
};

struct ScaleKeyframe
{
	double time;
	glm::vec3 scale;
	ScaleKeyframe(double time, glm::vec3 scale) : time(time), scale(scale) {}
};

struct AnimationKeyframes
{
	std::vector<PositionKeyframe> positionKeyframes;
	std::vector<RotationKeyframe> rotationKeyframes;
	std::vector<ScaleKeyframe>    scaleKeyframes;

	void AddNewPositionKeyframe(double time, glm::vec3 position);
	void AddNewRotationKeyframe(double time, glm::quat rotation);
	void AddNewScaleKeyframe(double time, glm::vec3 scale);
	
};

struct AnimationPoses
{
	AnimationPoses(std::map<std::string, AnimationKeyframes> keyframesMap) : keyframesMap(keyframesMap) {}
	AnimationPoses() {}
	// map between bone name and SQT transform
	std::map<std::string, AnimationKeyframes> keyframesMap;

	void AddNewPose(std::string boneName, AnimationKeyframes keyframes);

};

class AnimationClip
{
private:
	std::string clipName;
	Skeleton* skeleton; //TODO pointer has to be replaced with skeletonUnique Id
	double framePerSecond;
	int frameCount;

	/*
	 * Contains the poses in this animation clip.
	 * simply a map of name of the bone and the key frames related to that bone
	 */
	AnimationPoses animationPoses;
	
	bool loop;


public:
	AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop);
	AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop, AnimationPoses animationPoses);
	
	void SetClipName(std::string name);
	void SetSkeleton(Skeleton* skeleton);
	void SetFramePerSecond(double framePerSecond);
	void SetFrameCount(int frameCount);
};

class AnimationClipManager
{
public:
	AnimationClipManager(std::string path, Skeleton& skeleton);

private:

	Skeleton* skeleton;
	std::vector<AnimationClip* > loadedAnimationClips; //TODO change to map of string(name of the clip) and clip;
	
	void LoadAnimationClips(const aiScene* scene);
	AnimationKeyframes ProcessAnimationChannel(aiNodeAnim* channel);
};