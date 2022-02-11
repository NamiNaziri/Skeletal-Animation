#pragma once
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
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType, const std::string& textureName);


	std::vector<Vertex> ProcessVertices(aiMesh* mesh);
	std::vector<unsigned int> ProcessIndices(aiMesh* mesh);
	std::vector<Texture> ProcessMaterials(aiMesh* mesh, const aiScene* scene);
};

unsigned int LoadTextureFromFile(const std::string& path, const std::string& directory, bool gamma = false);