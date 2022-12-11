#pragma once
#include "Model.h"



class Bone
{
private:

	/** 
	 *
	 * This matrix describes the position of the mesh
	 * in the local space of this bone when the skeleton was bound.
	 * Thus it can be used directly to determine a desired vertex position,
	 * given the world-space transform of the bone when animated,
	 * and the position of the vertex in mesh space.
	 *
	 * It is sometimes called an inverse-bind matrix,
	 * or inverse bind pose matrix.
	 */
	glm::mat4 inverseBindPose;

	/** The transformation relative to the node's parent. */
	glm::mat4 transform;

	std::string	name;
	int parentIndex = -1;
	std::vector<std::shared_ptr<Bone>> children;

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

	std::vector<std::shared_ptr<Bone> >& GetChildren() { return children; }
	void AddNewChild(std::shared_ptr<Bone>  child) { children.push_back(child); }
	void SetTransform(glm::mat4 transform) { this->transform = transform; }
	void SetName(std::string& name) { this->name = name; }
};

class Skeleton
{
private:
	int uniqueID = -1;
	std::shared_ptr<Bone> rootBone;
	std::vector<std::shared_ptr<Bone> > bones;
public:
	Skeleton()
	{

	}

	~Skeleton()
	{

	}

	std::vector<std::shared_ptr<Bone>>& GetBones()
	{
		return bones;
	}

	void AddBone(std::shared_ptr<Bone> bone) { bones.push_back(bone); }
	void SetRootBone(std::shared_ptr<Bone> bone) { rootBone = bone; };
	std::shared_ptr<Bone> GetRootBone() { return rootBone; }

	int GetBoneIndexByName(const std::string& boneName)
	{
		for (int i = 0; i < bones.size(); i++)
		{
			if (bones[i]->GetName() == boneName)
			{
				return i;
			}
		}
		std::cout << "Bone not Found" << std::endl;
		return -1;
	}

	std::shared_ptr<Bone> GetBoneByName(const std::string& boneName)
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
	std::shared_ptr<Skeleton> GetSkeleton() { return skeleton; }
	
	void Draw(Shader& shader) override;
	void DrawSkeletonJoints(Shader& shader);
protected:
	std::shared_ptr<Skeleton> skeleton;
	
	std::vector<Model> skeletalJointsModels;
	
	void LoadAssets(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene) override;
	std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) override;

	
	std::vector<SkinnedVertex> ProcessSkinnedMeshVertices(aiMesh* mesh);
	void ProcessVerticesBoneWeight(aiMesh* mesh, SkinnedMesh& skinnedMesh);

	void SetDrawSkeleton(bool drawSkeleton);
private:
	/*
	 * used for finding the skeleton
	 */
	std::map<aiNode*, bool> necessityMap;
	std::map<std::string, std::shared_ptr<Bone> > boneInfoMap; // name of the bone -> BoneInfo

	std::vector<glm::mat4> skeletonTransform; // This just the skeleton transform
	std::vector<glm::mat4> matrixPalletTransform; 
	bool drawSkeleton = false;

	/*
	 * Source : http://assimp.sourceforge.net/lib_html/data.html
	 * So when creating the skeleton hierarchy for a mesh I suggest the following method:

		a) Create a map or a similar container to store which nodes are necessary for the skeleton. Pre-initialise it for all nodes with a "no".
		b) For each bone in the mesh:
		b1) Find the corresponding node in the scene's hierarchy by comparing their names.
		b2) Mark this node as "yes" in the necessityMap.
		b3) Mark all of its parents the same way until you 1) find the mesh's node or 2) the parent of the mesh's node.
		c) Recursively iterate over the node hierarchy
		c1) If the node is marked as necessary, copy it into the skeleton and check its children
		c2) If the node is marked as not necessary, skip it and do not iterate over its children.
	 */

	// Create a map to store which nodes are necessary for the skeleton
	void CreateNecessityMap(aiNode* node);
	
	/*
	   b) For each bone in the mesh:
			b1) Find the corresponding node in the scene's hierarchy by comparing their names.
			b2) Mark this node as "yes" in the necessityMap.
			b3) Mark all of its parents the same way until you 1) find the mesh's node or 2) the parent of the mesh's node.
	 */
	void ProcessNecessityMap(aiNode* node, const aiScene* scene);
	void ProcessNecessityMapHelper(aiMesh* mesh, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot); // finds the skeleton of the mesh
	void ProcessNecessityMapForEachBone(aiNode* node, aiNode* meshNode, aiNode* meshParentNode);

	
	
	// Animation and Bone Processing
	void CreateMeshSkeleton(aiNode* node);
	// recursively iterates over the hierarchy and creates the skeleton
	void CreateMeshSkeletonHelper(aiNode* node);

	void CreateJointModel();

	//TODO Draw skeleton bind pose!?
	
	void CalculateSkeletonTransform(std::shared_ptr<Bone>  root, glm::mat4 parentTrnsform);
	void CalculateMatrixPalletTransform(std::shared_ptr<Bone>  root, glm::mat4 parentTrnsform);
};
