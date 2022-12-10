#pragma once
#include "../EngineMath.h"
#include "vector"
#include "../SkeletalModel.h"
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
	// TODO it is assumed that the key frames are sorted based on the time
	std::vector<PositionKeyframe> positionKeyframes;
	std::vector<RotationKeyframe> rotationKeyframes;
	std::vector<ScaleKeyframe>    scaleKeyframes;

	void AddNewPositionKeyframe(double time, glm::vec3 position);
	void AddNewRotationKeyframe(double time, glm::quat rotation);
	void AddNewScaleKeyframe(double time, glm::vec3 scale);

	glm::vec3 GetPositionAtFrame(double Frame);
	glm::quat GetRotationAtFrame(double Frame);
	glm::vec3 GetScaleAtFrame(double Frame);
	
};

struct AnimationKeyframe
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

struct AnimationPoses
{
	AnimationPoses(std::map<std::string, AnimationKeyframes> keyframesMap) : keyframesMap(keyframesMap) {}
	AnimationPoses() {}
	// map between bone name and SQT transform
	std::map<std::string, AnimationKeyframes> keyframesMap;

	void AddNewBonePose(std::string boneName, AnimationKeyframes keyframes);

};

struct AnimationPose
{
	AnimationPose(std::map<std::string, AnimationKeyframe> keyframesMap) : keyframesMap(keyframesMap) {}
	AnimationPose() {}
	// map between bone name and SQT transform
	std::map<std::string, AnimationKeyframe> keyframesMap;
};

class AnimationClip
{
private:
	std::string name;
	Skeleton* skeleton; //TODO pointer has to be replaced with skeletonUnique Id
	double framePerSecond;
	int frameCount;
	double duration;
	/*
	 * Contains the poses in this animation clip.
	 * simply a map of name of the bone and the key frames related to that bone
	 */
	AnimationPoses animationPoses;
	
	bool loop;

	int loopCounter = 0;
public:
	AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop, double clipDuration);
	AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop, double clipDuration, AnimationPoses animationPoses);

	std::string GetName();
	
	void SetName(std::string name);
	void SetSkeleton(Skeleton* skeleton);
	void SetFramePerSecond(double framePerSecond);
	void SetFrameCount(int frameCount);

	AnimationPose GetPoseForCurrentFrame(double currentFrame);
	float GetDuration();
	double GetFramePerSecond();
};

