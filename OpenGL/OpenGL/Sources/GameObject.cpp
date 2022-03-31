#include "GameObject.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GameObject::GameObject(std::vector<float> vertices)
{
	this->vertices = vertices;
	SetupGameObject();
}

GameObject::~GameObject()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void GameObject::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void GameObject::SetAngle(float angle)
{
	this->angle = angle;
}

void GameObject::SetupGameObject()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	
	
}

void GameObject::Render(Shader& shader)
{
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	shader.SetMat4("model", model);

	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
	shader.SetMat3("normalMatrix",normalMatrix); 
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
