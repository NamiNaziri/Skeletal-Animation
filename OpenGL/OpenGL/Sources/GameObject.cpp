#include "GameObject.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


void GameObject::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void GameObject::SetAngle(float angle)
{
	this->angle = angle;
}

void GameObject::Translate(glm::vec3 dir, float speed)
{
	this->position += dir * speed;
}


GameObject::GameObject()
{
	model = glm::mat4(1.0f);
	position = glm::vec3(0.f);
	angle = 0;
}

void GameObject::Draw(Shader& shader)
{
	
	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	shader.SetMat4("model", model);

	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
	shader.SetMat3("normalMatrix",normalMatrix); 
	

}

const glm::vec3& GameObject::GetPosition()
{
	return position;
}

void SkeletalModelGameObject::Draw(Shader& shader)
{
	GameObject::Draw(shader);
	skeletalModel->Draw(shader);
}

SkeletalModelGameObject::SkeletalModelGameObject(SkeletalModel* skeletalModel)
	: skeletalModel(skeletalModel)
{
}
