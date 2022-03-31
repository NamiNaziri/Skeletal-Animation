#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>
class GameObject
{
private:
	unsigned int VAO;
	unsigned int VBO;
	std::vector<float> vertices;
	glm::mat4 model;
	glm::vec3 position;
	float angle;

public:
	GameObject(std::vector<float> vertices);
	~GameObject();
	void SetupGameObject();
	void Render(Shader& shader);


	void SetPosition(glm::vec3 position);
	void SetAngle(float angle);
	

	
};

