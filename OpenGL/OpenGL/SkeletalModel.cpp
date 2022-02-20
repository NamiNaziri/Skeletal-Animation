#include "SkeletalModel.h"
#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "STB/stb_image.h"
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

SkeletalModel::SkeletalModel(std::string path) :Model()
{
	LoadAssets(path);
}

void SkeletalModel::LoadAssets(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	CreateNecessityMap(scene->mRootNode);
	std::cout << "=========================================================================" << std::endl;

	ProcessNode(scene->mRootNode, scene);
	CreateMeshSkeleton(scene->mRootNode);

	glm::mat4 transformation; // your transformation matrix.
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	for(int i = 0 ; i < skeleton.GetBones().size() ; i++)
	{
		glm::decompose(skeleton.GetBones()[i]->GetInverseBindPose(), scale, rotation, translation, skew, perspective);

		std::cout << "Bone Name: " << skeleton.GetBones()[i]->GetName() << "  inv: " << glm::to_string(translation) << std::endl;
	}
	skeleton;
	if (scene->HasAnimations())
	{
		LoadAnimationClips(scene);
	}
}


void SkeletalModel::CreateNecessityMap(aiNode* node)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(ConvertMatrixToGLMFormat(node->mTransformation), scale, rotation, translation, skew, perspective);

	std::cout << "NodeName: " << node->mName.C_Str() << "  ||| Transformation: "<< glm::to_string(translation) << "  ||| ParentName: " << (node->mParent ? node->mParent->mName.C_Str() : std::string("No parent")) << std::endl;;
	necessityMap[node] = false;

	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		CreateNecessityMap(node->mChildren[j]);
	}
}

void SkeletalModel::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (mesh->HasBones())
		{
			ProcessNecessityMap(mesh, node, node->mParent, scene->mRootNode);
		}
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		ProcessNode(node->mChildren[j], scene);
	}
}

Mesh* SkeletalModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	Mesh* m = nullptr;

	// Processing indices
	const std::vector<unsigned int> indices = ProcessIndices(mesh);

	// Processing materials
	const std::vector<Texture> textures = ProcessMaterials(mesh, scene);
	
	if (mesh->HasBones()) 
	{
		// Process necessity map
		//ProcessNecessityMap(mesh);

		// process the vertices
		const std::vector<SkinnedVertex> vertices = ProcessSkinnedMeshVertices(mesh);

		// create the skinned mesh
		SkinnedMesh* sm = new SkinnedMesh(mesh->mName.C_Str(), vertices, indices, textures);

		// go through each bones of the mesh and assign each vertices boneWeight and boneIndex
		ProcessVerticesBoneWeight(mesh, *sm);
		m = sm;
	}
	else
	{
		std::cout << __FUNCTION__ << "This mesh does not contain any skeleton" << std::endl;
	}
	return m;
}


void SkeletalModel::LoadAnimationClips(const aiScene* scene)
{
	scene->mAnimations[0];
	for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
	{
		std::cout << "Channel: " << scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << std::endl;
	}
	scene->mAnimations[0]->mChannels[1];
}

std::vector<SkinnedVertex> SkeletalModel::ProcessSkinnedMeshVertices(aiMesh* mesh)
{
	std::vector<SkinnedVertex> vertices;

	//Processing mesh vertices (position, normal and tex coords)
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		SkinnedVertex sv;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}
		vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		vertex.biTangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

		sv.vertex = vertex;
		vertices.push_back(sv);
	}



	return vertices;
}

void SkeletalModel::ProcessVerticesBoneWeight(aiMesh* mesh, SkinnedMesh& skinnedMesh)
{
	std::vector<SkinnedVertex>& vertices = skinnedMesh.GetVertices();
	//Skeleton& skeleton = skinnedMesh.GetSkeleton();
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		const std::string boneName = bone->mName.C_Str();
		for (int j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight vertexWeight = bone->mWeights[j];
			const int availableIndex = vertices[vertexWeight.mVertexId].FindAvailableJointIndex();
			if (availableIndex == -1) //TODO look into the number of bones per vertex (maybe more than 4? )
			{
				continue;
			}
			vertices[vertexWeight.mVertexId].jointWeight[availableIndex] = vertexWeight.mWeight;
			vertices[vertexWeight.mVertexId].jointIndex[availableIndex] = skeleton.GetBoneIndexByName(boneName);
		}
	}
}



void SkeletalModel::ProcessNecessityMap(aiMesh* mesh, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot)
{
	for (unsigned i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		Bone* b = new Bone(ConvertMatrixToGLMFormat((bone->mOffsetMatrix)), bone->mName.C_Str());
		boneInfoMap[bone->mName.C_Str()] = b;
		ProcessNecessityMapForEachBone(bone->mNode, meshNode, meshParentNode, sceneRoot);

	}
}

void SkeletalModel::ProcessNecessityMapForEachBone(aiNode* node, aiNode* meshNode, aiNode* meshParentNode,
	aiNode* sceneRoot)
{
	if ((node == meshNode || node == meshParentNode || node == nullptr))
	{
		return;
	}
	necessityMap[node] = true;
	ProcessNecessityMapForEachBone(node->mParent, meshNode, meshParentNode, sceneRoot);
}

void SkeletalModel::CreateMeshSkeleton(aiNode* node)
{
	// Create
	CreateMeshSkeletonHelper(node);
	std::reverse(skeleton.GetBones().begin(), skeleton.GetBones().end());
	skeleton.SetRootBone(skeleton.GetBones()[0]);
}

void SkeletalModel::CreateMeshSkeletonHelper(aiNode* node)
{
	Bone* b = boneInfoMap[node->mName.C_Str()];

	// Bone transform
	if(b)
	{
		b->SetTransform(ConvertMatrixToGLMFormat(node->mTransformation));
	}
	
	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		if (necessityMap[node] == false)
		{
			continue;
		}
		if (b)
		{
			Bone* childBone = boneInfoMap[node->mChildren[j]->mName.C_Str()];
			if(childBone)
			{
				b->AddNewChild(childBone);
			}
			
		}
		CreateMeshSkeletonHelper(node->mChildren[j]);
	}
	if (b)
	{
		skeleton.AddBone(b);
	}
}


inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}