#include "AnimationClipManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Animation.h"


AnimationClipManager::AnimationClipManager(const std::string& path, std::shared_ptr<Skeleton> skeleton)
		:skeleton(skeleton)
{
	AddNewAnimationClip(path);
}

AnimationClipManager::AnimationClipManager(std::shared_ptr<Skeleton> skeleton) : skeleton(skeleton)
{}

AnimationClipManager::~AnimationClipManager()
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
		for (unsigned j = 0; j < anim->mNumChannels; j++)
		{
			AnimationKeyframes keyframes = ProcessAnimationChannel(anim->mChannels[j]);
			std::string boneName = anim->mChannels[j]->mNodeName.C_Str();
			poses.AddNewBonePose(boneName, keyframes);
		}
		
		// Creating an animation clip
		//TODO loop should be assigned from outside of this class. prob in main.cpp
		if(anim->mTicksPerSecond == 0.f)
		{
			std::cout<< __FUNCTION__ << "mTicksPerSecond: "<<anim->mTicksPerSecond << "(0 Division)" << std::endl;
			return;
		}
		std::shared_ptr<AnimationClip> animation = std::make_shared<AnimationClip>(anim->mName.C_Str(), skeleton,
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

std::vector<std::shared_ptr<AnimationClip> > AnimationClipManager::GetLoadedAnimationClips()
{
	return loadedAnimationClips;
}

std::shared_ptr<Skeleton> AnimationClipManager::GetSkeleton()
{
	return skeleton;
}
