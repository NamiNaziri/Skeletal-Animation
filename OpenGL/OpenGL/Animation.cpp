#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop)
	:clipName(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount), loop(loop)
{
}

AnimationClip::AnimationClip(std::string clipName, Skeleton* skeleton, double framePerSecond, int frameCount, bool loop,
	AnimationPoses animationPoses)
	: clipName(clipName), skeleton(skeleton), framePerSecond(framePerSecond), frameCount(frameCount),
		loop(loop), animationPoses(animationPoses)
{
}


void AnimationClip::SetClipName(std::string clipName)
{
	this->clipName = clipName;
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
		AnimationClip* animation = new AnimationClip(anim->mName.C_Str(), skeleton, anim->mTicksPerSecond, frameCount, false, poses);
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
