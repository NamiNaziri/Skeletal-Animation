#pragma once
#include "Model.h"


class SkeletalModel : public Model
{
public:
	SkeletalModel(std::string path);
	Skeleton& GetSkeleton() { return skeleton; }
protected:
	Skeleton skeleton;
	std::vector<AnimationClip> loadedAnimationClips;

	void LoadAssets(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene) override;
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene) override;

	void LoadAnimationClips(const aiScene* scene);
	std::vector<SkinnedVertex> ProcessSkinnedMeshVertices(aiMesh* mesh);
	void ProcessVerticesBoneWeight(aiMesh* mesh, SkinnedMesh& skinnedMesh);


private:
	/*
	 * used for finding the skeleton
	 */
	std::map<aiNode*, bool> necessityMap;
	std::map<std::string, Bone* > boneInfoMap; // name of the bone -> BoneInfo

	void CreateNecessityMap(aiNode* node);
	void ProcessNecessityMap(aiMesh* mesh, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot); // finds the skeleton of the mesh
	void ProcessNecessityMapForEachBone(aiNode* node, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot);

	// Animation and Bone Processing
	void CreateMeshSkeleton(aiNode* node);
	// recursively iterates over the hierarchy and creates the skeleton
	void CreateMeshSkeletonHelper(aiNode* node);
};

