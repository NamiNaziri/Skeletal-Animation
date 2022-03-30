#pragma once
#include <assimp/Importer.hpp>

#include "Animation.h"
#include "../SkeletalModel.h"

class AnimationClipManager
{
public:
	AnimationClipManager(const std::string& path, Skeleton& skeleton);
	AnimationClipManager(Skeleton& skeleton);
	void AddNewAnimationClip(const std::string& path);
private:

	Skeleton* skeleton;
	std::vector<AnimationClip* > loadedAnimationClips; //TODO change to map of string(name of the clip) and clip;
	void LoadAnimationClips(const aiScene* scene);
	AnimationKeyframes ProcessAnimationChannel(aiNodeAnim* channel);

public:
	std::vector<AnimationClip* > GetLoadedAnimationClips();
	Skeleton* GetSkeleton();

private:
	Assimp::Importer importer;
	
};
