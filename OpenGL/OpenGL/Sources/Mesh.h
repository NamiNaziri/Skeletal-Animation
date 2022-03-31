#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 biTangent;
	glm::vec2 texCoords;
};

struct SkinnedVertex
{
	Vertex vertex;
	//bone indexes which will influence this vertex
	int jointIndex[4] = {-1,-1,-1,-1};
	glm::vec4 jointWeight = glm::vec4(0.0f);
public:
	int FindAvailableJointIndex()
	{
		
		for(int i = 0; i < 4 ; i++)
		{
			if(jointIndex[i] == -1)
			{
				return i;
			}
		}

		return -1;
	}
};




enum TextureType{ diffuse, specular, normal };

struct Texture
{
	unsigned int id;
	std::string name;
	TextureType type;
	std::string path;
};


class Mesh
{
public:
	Mesh(std::string name, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);

public:
	void SetupMesh();

protected:
	std::string name;

	
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int VBO, VAO, EBO;
};

//TODO
class StaticMesh:public Mesh
{
public:
	StaticMesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

protected:
	void SetupMesh();

protected:
	std::vector<Vertex> vertices; // this var should be different based on diff types of Meshes
};

class SkinnedMesh:public Mesh
{
public:
	SkinnedMesh(std::string name,std::vector<SkinnedVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	std::vector<SkinnedVertex>& GetVertices()
	{
		return vertices;
	}

public:
	void SetupMesh();

protected:
	std::vector<SkinnedVertex> vertices;

};