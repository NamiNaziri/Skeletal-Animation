#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop, double duration)
	:name(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount),loop(loop), duration(duration)
{
	loopCounter = 0;
}

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop, double duration,
	AnimationPoses animationPoses)
	: name(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount), loop(loop), duration(duration),
		animationPoses(animationPoses)
{
	loopCounter = 0;
}

std::string AnimationClip::GetName()
{
	return name;
}


void AnimationClip::SetName(std::string clipName)
{
	this->name = clipName;
}

void AnimationClip::SetSkeleton(Skeleton* skeleton)
{
	this->skeleton = skeleton;
}

void AnimationClip::SetFramePerSecond(double framePerSecond)
{
	this->framePerSecond = framePerSecond;
}

void AnimationClip::SetFrameCount(int frameCount)
{
	this->frameCount = frameCount;
}

AnimationPose AnimationClip::GetPoseForCurrentTime(double currentTime)
{
	if(currentTime > duration)
	{
		if(loop)
		{
			//TODO loop over the animation
			//loopCounter++;
			while(currentTime - duration > 0)
			{
				currentTime = currentTime - duration;
			}
			
			
		}
		else
		{
			//TODO return the last pose of the anim
		}
		
	}
	AnimationPose pose;
	for (auto x : animationPoses.keyframesMap)
	{
		AnimationKeyframe keyframe;
		keyframe.position = x.second.GetPositionAtTime(currentTime);
		keyframe.rotation = x.second.GetRotationAtTime(currentTime);
		keyframe.scale = x.second.GetScaleAtTime(currentTime);
		pose.keyframesMap.emplace(x.first, keyframe);
	}
	
	return pose;
	
}

float AnimationClip::GetDuration()
{
	return duration;
}


void AnimationKeyframes::AddNewPositionKeyframe(double time, glm::vec3 position)
{
	this->positionKeyframes.emplace_back(time, position);
}

void AnimationKeyframes::AddNewRotationKeyframe(double time, glm::quat rotation)
{
	this->rotationKeyframes.emplace_back(time, rotation);
}

void AnimationKeyframes::AddNewScaleKeyframe(double time, glm::vec3 scale)
{
	this->scaleKeyframes.emplace_back(time, scale);
}

glm::vec3 AnimationKeyframes::GetPositionAtTime(double time)
{
	if( positionKeyframes.size() == 1)
	{
		return positionKeyframes[0].position;
	}
	if(time > positionKeyframes.back().time )
	{
		std::cout << __FUNCTION__ << "time cant be more than the anim duration";
		return glm::vec3(0,0,0);
	}
	for(int i = 1 ; i < positionKeyframes.size() ; i++)
	{
		if(  positionKeyframes[i].time > time)
		{
			
			PositionKeyframe currentPos = positionKeyframes[i - 1];
			PositionKeyframe nextPos = positionKeyframes[i];
			return EngineMath::LinearBlend(currentPos.position, nextPos.position,
				currentPos.time, time, nextPos.time
			);
		}
	}
	std::cout << __FUNCTION__ << "Something went wrong";
	return glm::vec3(0, 0, 0);
}

glm::quat AnimationKeyframes::GetRotationAtTime(double time)
{

	if (rotationKeyframes.size() == 1)
	{
		return rotationKeyframes[0].rotation;
	}

	
	if (time > rotationKeyframes.back().time)
	{
		std::cout << __FUNCTION__ << "time cant be more than the anim duration";
		return glm::quat();
	}
	for (int i = 1; i < rotationKeyframes.size(); i++)
	{
		if (rotationKeyframes[i].time > time)
		{

			RotationKeyframe currentRot = rotationKeyframes[i - 1];
			RotationKeyframe nextRot = rotationKeyframes[i];

			const float alpha = (time - currentRot.time) / (nextRot.time - currentRot.time);
			glm::quat ans = glm::slerp(currentRot.rotation, nextRot.rotation, alpha);
			return ans;
		}
	}
	std::cout << __FUNCTION__ << "Something went wrong";
	return glm::quat();
}

glm::vec3 AnimationKeyframes::GetScaleAtTime(double time) //TODO
{
	return glm::vec3();
}

void AnimationPoses::AddNewPose(std::string boneName, AnimationKeyframes keyframes)
{
	this->keyframesMap.emplace(boneName, keyframes);
}





AnimationClipManager::AnimationClipManager(std::string path, Skeleton& skeleton) :skeleton(&skeleton)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	// Loading animation clips
	if (scene->HasAnimations())
	{
		LoadAnimationClips(scene);
	}
	std::cout << std::endl << loadedAnimationClips.size() << " Number of Animation added" << std::endl;

}

void AnimationClipManager::LoadAnimationClips(const aiScene* scene)
{
	for (unsigned i = 0; i < scene->mNumAnimations; i++)
	{
		// Getting the animation reference
		aiAnimation* anim = scene->mAnimations[i];

		// Calculating frame Count
		int frameCount = static_cast<int>(anim->mDuration * anim->mTicksPerSecond);

		// Create animation poses
		AnimationPoses poses;
		for (unsigned i = 0; i < anim->mNumChannels; i++)
		{
			AnimationKeyframes keyframes = ProcessAnimationChannel(anim->mChannels[i]);
			std::string boneName = anim->mChannels[i]->mNodeName.C_Str();
			poses.AddNewPose(boneName, keyframes);
		}

		// Creating an animation clip
		//TODO loop should be assigned from outside of this class. prob in main.cpp
		AnimationClip* animation = new AnimationClip(anim->mName.C_Str(), skeleton, anim->mTicksPerSecond, frameCount, true ,anim->mDuration, poses);
		loadedAnimationClips.push_back(animation);
	}
}

AnimationKeyframes AnimationClipManager::ProcessAnimationChannel(aiNodeAnim* channel)
{
	AnimationKeyframes animKeyframes;

	for (unsigned i = 0; i < channel->mNumPositionKeys; i++)
	{
		aiVectorKey posKey = channel->mPositionKeys[i];
		animKeyframes.AddNewPositionKeyframe(posKey.mTime, EngineMath::GetGLMVec(posKey.mValue));
	}

	for (unsigned i = 0; i < channel->mNumRotationKeys; i++)
	{
		aiQuatKey rotKey = channel->mRotationKeys[i];
		animKeyframes.AddNewRotationKeyframe(rotKey.mTime, EngineMath::GetGLMQuat(rotKey.mValue));
	}

	for (unsigned i = 0; i < channel->mNumScalingKeys; i++)
	{
		aiVectorKey scaleKey = channel->mScalingKeys[i];
		animKeyframes.AddNewScaleKeyframe(scaleKey.mTime, EngineMath::GetGLMVec(scaleKey.mValue));
	}

	return animKeyframes;
}

std::vector<AnimationClip* > AnimationClipManager::GetLoadedAnimationClips()
{
	return loadedAnimationClips;
}

Skeleton* AnimationClipManager::GetSkeleton()
{
	return skeleton;
}
