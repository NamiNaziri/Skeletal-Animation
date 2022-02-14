#pragma once
#include <map>

#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include <assimp/scene.h>

class Model
{
public:
	Model(std::string path);

	void Draw(Shader& shader);
	
protected:
	std::vector<Mesh*> meshes;
	std::string directory;
	std::vector<Texture> loadedTextures;


	void LoadModel(std::string path);
	void CreateRelationshipMap(aiNode* node);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(const aiScene* scene,aiMaterial* mat, aiTextureType type, TextureType textureType, const std::string& textureName);


	std::vector<Vertex> ProcessStaticMeshVertices(aiMesh* mesh);
	std::vector<SkinnedVertex> ProcessSkinnedMeshVertices(aiMesh* mesh);
	void ProcessVerticesBoneWeight(aiMesh* mesh, SkinnedMesh& skinnedMesh);
	std::vector<unsigned int> ProcessIndices(aiMesh* mesh);
	std::vector<Texture> ProcessMaterials(aiMesh* mesh, const aiScene* scene);



	// Animation and Bone Processing
	Skeleton CreateMeshSkeleton(aiMesh* mesh);

private:
	/*
	 * Holds child parent relationship in the scene
	 * NULL means no parent
	 */
	std::map<std::string, std::string> relationMap; 
};

unsigned int LoadTextureFromFile(const std::string& path, const std::string& directory, bool gamma = false);