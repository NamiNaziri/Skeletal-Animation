#pragma once
#include "Model.h"



class Bone
{
private:
	glm::mat4 inverseBindPose;
	glm::mat4 transform;

	std::string	name;
	int parentIndex = -1;
	std::vector<Bone*> children;

public:
	Bone(glm::mat4 invBindPose, std::string name)
		: inverseBindPose(invBindPose), name(name)
	{
	}

	void SetParentIndex(int index)
	{
		parentIndex = index;
	}

	std::string GetName() { return name; }
	int GetParentIndex() { return parentIndex; }
	glm::mat4 GetInverseBindPose() { return inverseBindPose; }
	glm::mat4 GetTransform() { return transform; }

	std::vector<Bone*>& GetChildren() { return children; }
	void AddNewChild(Bone* child) { children.push_back(child); }
	void SetTransform(glm::mat4 transform) { this->transform = transform; }
};

class Skeleton
{
private:
	int uniqueID = -1;
	Bone* rootBone;
	std::vector<Bone* > bones;
public:
	Skeleton()
	{

	}

	std::vector<Bone*>& GetBones()
	{
		return bones;
	}

	void AddBone(Bone* bone) { bones.push_back(bone); }
	void SetRootBone(Bone* bone) { rootBone = bone; };
	Bone* GetRootBone() { return rootBone; }

	int GetBoneIndexByName(const std::string& boneName)
	{
		for (int i = 0; i < bones.size(); i++)
		{
			if (bones[i]->GetName() == boneName)
			{
				return i;
			}
		}

		return -1;
	}

	Bone* GetBoneByName(const std::string& boneName)
	{
		for (int i = 0; i < bones.size(); i++)
		{
			if (bones[i]->GetName() == boneName)
			{
				return bones[i];
			}
		}

		return nullptr;
	}
	void Print()
	{
		for (int i = 0; i < bones.size(); i++)
		{
			int pIndex = 0;
			std::string parent;
			if (bones[i]->GetParentIndex() < 0)
			{
				parent = "No Parent";
			}
			else
			{
				parent = bones[bones[i]->GetParentIndex()]->GetName();
			}

			std::cout << "Bone Name: " << bones[i]->GetName() << " Parent Name: " << parent << std::endl;
		}
	}
};

class Skeleton;



class SkeletalModel : public Model
{
public:
	SkeletalModel(std::string path);
	Skeleton& GetSkeleton() { return skeleton; }
	
	void Draw(Shader& shader) override;
	void DrawSkeletonJoints(Shader& shader);
protected:
	Skeleton skeleton;
	
	std::vector<Model> skeletalJointsModels;
	
	void LoadAssets(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene) override;
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene) override;

	
	std::vector<SkinnedVertex> ProcessSkinnedMeshVertices(aiMesh* mesh);
	void ProcessVerticesBoneWeight(aiMesh* mesh, SkinnedMesh& skinnedMesh);

	void SetDrawSkeleton(bool drawSkeleton);
private:
	/*
	 * used for finding the skeleton
	 */
	std::map<aiNode*, bool> necessityMap;
	std::map<std::string, Bone* > boneInfoMap; // name of the bone -> BoneInfo
	std::vector<glm::vec3> skeletonPosition;
	bool drawSkeleton = false;

	void CreateNecessityMap(aiNode* node);
	void ProcessNecessityMap(aiMesh* mesh, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot); // finds the skeleton of the mesh
	void ProcessNecessityMapForEachBone(aiNode* node, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot);

	// Animation and Bone Processing
	void CreateMeshSkeleton(aiNode* node);
	// recursively iterates over the hierarchy and creates the skeleton
	void CreateMeshSkeletonHelper(aiNode* node);

	void CreateJointModel();

	//TODO Draw skeleton bind pose!?
	
	void DrawSkeletonJointsHelper(Bone* root, glm::mat4 parentTrnsform);
};
