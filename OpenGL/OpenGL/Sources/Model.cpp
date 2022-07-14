#include "Model.h"

#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "STB/stb_image.h"
#include <glm/gtx/matrix_decompose.hpp>




Model::Model(std::string path)
{
	LoadAssets(path);
}

Model::Model()
{
	//LoadAssets(path);
}

void Model::Draw(Shader& shader)
{
	for (auto mesh : meshes)
	{
		if(mesh) //TODO currently because we only create skeletal model and some models might be static. in this case the mesh will be null
		{
			mesh->Draw(shader);
		}
		
	}
}

void Model::LoadAssets(std::string path)
{
	if(path != "resources/objects/Sphere/sphere.obj")
	{
		std::cout << "hi";
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace );
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}


void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	
	for(unsigned int i = 0 ; i < node->mNumMeshes ; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for(unsigned int j = 0 ; j < node->mNumChildren ; j++)
	{
		ProcessNode(node->mChildren[j], scene);
	}
}

Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{

	Mesh* m = nullptr;
	
	// Processing indices
	const std::vector<unsigned int> indices = ProcessIndices(mesh);

	// Processing materials
	const std::vector<Texture> textures = ProcessMaterials(mesh, scene);
	
	// Processing Vertices

	const std::vector<Vertex> vertices = ProcessStaticMeshVertices(mesh);
		
	m = new StaticMesh(mesh->mName.C_Str(), vertices, indices, textures);

	return m;
}


std::vector<Vertex> Model::ProcessStaticMeshVertices(aiMesh* mesh)
{
	std::vector<Vertex> vertices;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
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

		vertices.push_back(vertex);
	}
	return vertices;
}


std::vector<unsigned int> Model::ProcessIndices(aiMesh* mesh) 
{
	std::vector<unsigned int> indices;
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	return indices;
}

std::vector<Texture> Model::ProcessMaterials(aiMesh* mesh, const aiScene* scene) 
{
	std::vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(scene, material, aiTextureType_DIFFUSE, TextureType::diffuse, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = LoadMaterialTextures(scene, material, aiTextureType_SPECULAR, TextureType::specular, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


		//TODO fix this. find the difference between HEIGHT and NORMALS
		std::vector<Texture> normalMaps = LoadMaterialTextures(scene, material, aiTextureType_HEIGHT, TextureType::normal, "texture_normal");
		//std::vector<Texture> normalMaps = LoadMaterialTextures(scene, material, aiTextureType_NORMALS, TextureType::normal, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return textures;
}

// todo fix texture import
std::vector<Texture> Model::LoadMaterialTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type, TextureType textureType, const std::string& textureName)
{
	std::vector<Texture> textures;

	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str); // Get the texture path

		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++) // Check if its already loaded using the path of the texture
		{
			if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it

			Texture texture;
			
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture) // Check if the current texture is an embedded one
			{
				texture.id = LoadEmbeddedTexture(embeddedTexture,str.C_Str());
			}
			else // Its not embedded, so load the external texture using the path
			{
				
				texture.id = LoadTextureFromFile(str.C_Str(), directory);
				
			}
			
			texture.type = textureType;
			texture.name = textureName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			loadedTextures.push_back(texture); // add to loaded textures
		}
	}


	return textures;
}

unsigned int LoadEmbeddedTexture(const aiTexture* embeddedTexture, const std::string& path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	
	unsigned char* image_data = nullptr;
	int width, height, nrComponents;
	
	if (embeddedTexture->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth, &width, &height, &nrComponents, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth * embeddedTexture->mHeight, &width, &height, &nrComponents, 0);
	}

	if (image_data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image_data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(image_data);
	}
	return textureID;
}



unsigned int LoadTextureFromFile(const std::string& path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

