#include "Mesh.h"

Mesh::Mesh(std::string name, std::vector<unsigned> indices, std::vector<Texture> textures)
{
	this->name = name;
	this->indices = indices;
	this->textures = textures;
	
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	for(int i = 0 ; i < textures.size() ; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		if(textures[i].type == TextureType::diffuse)
		{
			number = std::to_string(diffuseNr);
			diffuseNr++;
		}
		else if (textures[i].type == TextureType::specular)
		{
			number = std::to_string(specularNr);
			specularNr++;
		}
		else if (textures[i].type == TextureType::normal)
		{
			number = std::to_string(normalNr);
			normalNr++;
		}

		shader.SetInt(("material." + textures[i].name + number), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	

	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	
}

void Mesh::SetupMesh()
{
}

StaticMesh::StaticMesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures)
	: Mesh(name,indices,textures)
{
	this->vertices = vertices;
	SetupMesh();
}

void StaticMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);	
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biTangent));

	glBindVertexArray(0);
}


//////////////////////////////////////

SkinnedMesh::SkinnedMesh(std::string name, std::vector<SkinnedVertex> vertices, std::vector<unsigned> indices,
	std::vector<Texture> textures): Mesh(name,indices,textures)
{
	this->vertices = vertices;
	SetupMesh();
}

void SkinnedMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(Vertex, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(Vertex, biTangent));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, jointIndex));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, jointWeight));
	
	glBindVertexArray(0);
}
