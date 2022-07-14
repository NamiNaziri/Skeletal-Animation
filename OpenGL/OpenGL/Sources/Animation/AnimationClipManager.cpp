#include "AnimationClipManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Animation.h"


AnimationClipManager::AnimationClipManager(const std::string& path, Skeleton& skeleton)
		:skeleton(&skeleton)
{
	AddNewAnimationClip(path);
}

AnimationClipManager::AnimationClipManager(Skeleton& skeleton) : skeleton(&skeleton)
{}

void AnimationClipManager::AddNewAnimationClip(const std::string& path)
{
	const aiScene* scene = importer.ReadFile(path, aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights);
	if (!scene || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	// Loading animation clips
	if (scene->HasAnimations())
	{
		LoadAnimationClips(scene);
	}
}

void AnimationClipManager::LoadAnimationClips(const aiScene* scene)
{
	for (unsigned i = 0; i < scene->mNumAnimations; i++)
	{
		// Getting the animation reference
		aiAnimation* anim = scene->mAnimations[i];

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
		AnimationClip* animation = new AnimationClip(anim->mName.C_Str(), skeleton,
			anim->mTicksPerSecond, anim->mDuration, true, anim->mDuration / anim->mTicksPerSecond, poses);
		loadedAnimationClips.push_back(animation);
		std::cout << std::endl << "New Animation added. Clip name: " << anim->mName.C_Str() << std::endl;

	}
}

AnimationKeyframes AnimationClipManager::ProcessAnimationChannel(aiNodeAnim* channel)
{
 	AnimationKeyframes animKeyframes;
	double startTime = 0; // make sure every channel starts at zero 
	if(channel->mNumPositionKeys > 0)
	{
		startTime = channel->mPositionKeys[0].mTime; 
	}
	
	for (unsigned i = 0; i < channel->mNumPositionKeys; i++)
	{
		aiVectorKey posKey = channel->mPositionKeys[i];
		animKeyframes.AddNewPositionKeyframe(posKey.mTime - startTime, EngineMath::GetGLMVec(posKey.mValue));
	}
	if(channel->mNumRotationKeys > 0)
	{
		startTime = channel->mRotationKeys[0].mTime;
	}
	
	for (unsigned i = 0; i < channel->mNumRotationKeys; i++)
	{
		aiQuatKey rotKey = channel->mRotationKeys[i];
		animKeyframes.AddNewRotationKeyframe(rotKey.mTime - startTime, EngineMath::GetGLMQuat(rotKey.mValue));
	}

	if (channel->mNumScalingKeys > 0)
	{
		startTime = channel->mScalingKeys[0].mTime;
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
