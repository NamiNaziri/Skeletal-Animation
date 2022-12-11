#pragma once
#include <assimp/Importer.hpp>

#include "Animation.h"
#include "../SkeletalModel.h"

class AnimationClipManager
{
public:
	AnimationClipManager(const std::string& path, std::shared_ptr<Skeleton> skeleton);
	AnimationClipManager(std::shared_ptr<Skeleton> skeleton);
	~AnimationClipManager();
	void AddNewAnimationClip(const std::string& path);
	
private:

	std::shared_ptr<Skeleton> skeleton;
	std::vector<std::shared_ptr<AnimationClip>> loadedAnimationClips; //TODO change to map of string(name of the clip) and clip;
	void LoadAnimationClips(const aiScene* scene);
	AnimationKeyframes ProcessAnimationChannel(aiNodeAnim* channel);

public:
	std::vector<std::shared_ptr<AnimationClip> > GetLoadedAnimationClips();
	std::shared_ptr<AnimationClip> GetAnimationClipByName(std::string animName);  //TODO 
	std::shared_ptr<Skeleton> GetSkeleton();

private:
	Assimp::Importer importer;
	
};
