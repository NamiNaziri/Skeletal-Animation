#pragma once
#include <map>

#include "Mesh.h"
#include "Shader.h"
#include <vector>
#include <assimp/scene.h>

/*
 * right now Model class is being used as asset manager and scene manager. TODO has to be changed later.
 */
class Model
{
public:
	Model(std::string path);
	Model();
	~Model();
	virtual void Draw(Shader& shader);
	
protected:
	std::vector<Mesh*> meshes;
	std::string directory;
	std::vector<Texture> loadedTextures;

	

	
	void LoadAssets(std::string path);
	 
	virtual void ProcessNode(aiNode* node, const aiScene* scene);
	virtual Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(const aiScene* scene,aiMaterial* mat, aiTextureType type, TextureType textureType, const std::string& textureName);


	std::vector<Vertex> ProcessStaticMeshVertices(aiMesh* mesh);
	
	
	std::vector<unsigned int> ProcessIndices(aiMesh* mesh);
	std::vector<Texture> ProcessMaterials(aiMesh* mesh, const aiScene* scene);



};
unsigned int LoadEmbeddedTexture(const aiTexture* embeddedTexture, const std::string& path);
unsigned int LoadTextureFromFile(const std::string& path, const std::string& directory, bool gamma = false);