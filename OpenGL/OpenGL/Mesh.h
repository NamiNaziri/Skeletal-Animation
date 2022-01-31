#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

enum TextureType{ diffuse, specular };

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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);

private:
	void SetupMesh();
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int VBO, VAO, EBO;
};

