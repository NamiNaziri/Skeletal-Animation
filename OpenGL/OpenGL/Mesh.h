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

	glm::vec4 jointIndex;
	glm::vec4 jointWeight;
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
	Mesh(std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);

protected:
	virtual void SetupMesh();

protected:
	
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int VBO, VAO, EBO;
};

//TODO
class StaticMesh:public Mesh
{
public:
	StaticMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

protected:
	void SetupMesh() override;

protected:
	std::vector<Vertex> vertices; // this var should be different based on diff types of Meshes
};

class SkinnedMesh:public Mesh
{
public:
	SkinnedMesh(std::vector<SkinnedVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	
protected:
	void SetupMesh() override;

protected:
	std::vector<SkinnedVertex> vertices;
};