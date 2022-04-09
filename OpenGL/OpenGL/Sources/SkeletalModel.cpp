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
#include "EngineMath.h"

SkeletalModel::SkeletalModel(std::string path) :Model()
{
	LoadAssets(path);
}

void SkeletalModel::Draw(Shader& shader)
{
	//TODO Process the skeleton here

	//TODO the actual function of creating pallet should clear the matrixPalletTransform too.
	matrixPalletTransform.clear();
	matrixPalletTransform = std::vector<glm::mat4>(skeleton.GetBones().size());
	CalculateMatrixPalletTransform(skeleton.GetRootBone(), glm::mat4(1.0f));

	for(int i = 0 ; i < skeleton.GetBones().size() ; i++)
	{
		std::string str = "JointsTransformation[" + std::to_string(i) + "]";
		shader.SetMat4(str, matrixPalletTransform[i]);
	}

	
	Model::Draw(shader);

	if(drawSkeleton) // TODO ?? is it necessary?
	{
		
	}

	
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

	//Fill up the necessity map
	ProcessNecessityMap(scene->mRootNode, scene);

	
	// Creating Meshes' bind Skeleton
	CreateMeshSkeleton(scene->mRootNode);

	// Process meshes and the bone coresponding to each mesh
	ProcessNode(scene->mRootNode, scene);

	
	CreateJointModel();

	
	//TODO debug stuff. delete this or refactor it!
	glm::mat4 transformation; // your transformation matrix.
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	for(unsigned i = 0 ; i < skeleton.GetBones().size() ; i++)
	{
		glm::decompose(skeleton.GetBones()[i]->GetInverseBindPose(), scale, rotation, translation, skew, perspective);

		std::cout << "Bone Name: " << skeleton.GetBones()[i]->GetName() << "  inv: " << glm::to_string(translation) << std::endl;
	}


	
}


void SkeletalModel::CreateNecessityMap(aiNode* node)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(EngineMath::ConvertMatrixToGLMFormat(node->mTransformation), scale, rotation, translation, skew, perspective);

	std::cout << "NodeName: " << node->mName.C_Str() << "  ||| Transformation: "<< glm::to_string(translation) << "  ||| ParentName: " << (node->mParent ? node->mParent->mName.C_Str() : std::string("No parent")) << std::endl;;
	necessityMap[node] = false;

	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		CreateNecessityMap(node->mChildren[j]);
	}
}

void SkeletalModel::ProcessNecessityMap(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (mesh->HasBones())
		{
			ProcessNecessityMapHelper(mesh, node, node->mParent, scene->mRootNode);
		}

	}

	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		ProcessNecessityMap(node->mChildren[j], scene);
	}
}

void SkeletalModel::ProcessNecessityMapHelper(aiMesh* mesh, aiNode* meshNode, aiNode* meshParentNode, aiNode* sceneRoot)
{
	for (unsigned i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];

		Bone* b = new Bone(EngineMath::ConvertMatrixToGLMFormat((bone->mOffsetMatrix)), bone->mName.C_Str());
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


void SkeletalModel::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
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
		

		// process the vertices
		const std::vector<SkinnedVertex> vertices = ProcessSkinnedMeshVertices(mesh);

		// create the skinned mesh
		SkinnedMesh* sm = new SkinnedMesh(mesh->mName.C_Str(), vertices, indices, textures);
		
		// go through each bones of the mesh and assign each vertices boneWeight and boneIndex
		ProcessVerticesBoneWeight(mesh, *sm);
		sm->SetupMesh();
		m = sm;
	}
	else
	{
		std::cout << __FUNCTION__ << "This mesh does not contain any skeleton" << std::endl;
	}
	return m;
}

void SkeletalModel::DrawSkeletonJoints(Shader& shader)
{

	skeletonTransform.clear();
	CalculateSkeletonTransform(skeleton.GetRootBone(), glm::mat4(1.0f));

	for (int i = 0; i < skeleton.GetBones().size(); i++)
	{
		shader.SetMat4("model", skeletonTransform[i]);
		const glm::mat3 normalMatrix = glm::transpose(glm::inverse(skeletonTransform[i]));
		shader.SetMat3("normalMatrix", normalMatrix);
		skeletalJointsModels[i].Draw(shader);
	}

	
}

void SkeletalModel::CalculateSkeletonTransform(Bone* root, glm::mat4 parentTrnsform)
{
	const glm::mat4 newParentTransform = parentTrnsform * root->GetTransform();

	skeletonTransform.push_back(newParentTransform);
	for (int i = 0; i < root->GetChildren().size(); i++)
	{
		CalculateSkeletonTransform(root->GetChildren()[i], newParentTransform);
	}
}

void SkeletalModel::CalculateMatrixPalletTransform(Bone* root, glm::mat4 parentTrnsform)
{
	// TODO is the order right?
	const glm::mat4 newParentTransform = (parentTrnsform * root->GetTransform());

	const int boneIndex = skeleton.GetBoneIndexByName(root->GetName());
	matrixPalletTransform[boneIndex] =  newParentTransform * root->GetInverseBindPose();
	
	//matrixPalletTransform.push_back(newParentTransform * root->GetInverseBindPose());
	for (int i = 0; i < root->GetChildren().size(); i++)
	{
		CalculateMatrixPalletTransform(root->GetChildren()[i], newParentTransform);
	}
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
		const int boneIndex = skeleton.GetBoneIndexByName(boneName);
		for (int j = 0; j < bone->mNumWeights; j++)
		{
			const aiVertexWeight vertexWeight = bone->mWeights[j];
			const int availableIndex = vertices[vertexWeight.mVertexId].FindAvailableJointIndex();
			if (availableIndex == -1) //TODO look into the number of bones per vertex (maybe more than 4? )
			{
				continue;
			}
			vertices[vertexWeight.mVertexId].jointWeight[availableIndex] = vertexWeight.mWeight;
			vertices[vertexWeight.mVertexId].jointIndex[availableIndex] = boneIndex;
		}
	}
}

void SkeletalModel::SetDrawSkeleton(bool drawSkeleton)
{
	this->drawSkeleton = drawSkeleton;
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
		b->SetTransform(EngineMath::ConvertMatrixToGLMFormat(node->mTransformation));
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

void SkeletalModel::CreateJointModel()
{
	for (int i = 0; i < skeleton.GetBones().size(); i++)
	{
		Model secModel("resources/objects/Sphere/sphere.obj");
		skeletalJointsModels.push_back(secModel);

	}
	
}
