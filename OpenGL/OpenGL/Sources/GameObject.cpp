#include "GameObject.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <cmath>
#include <glm/glm.hpp>

void GameObject::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void GameObject::SetScale(glm::vec3 scale)
{
	this->scale = scale;
}

void GameObject::SetAngle(float _angle)
{
	SetRotation(_angle, this->rotationVector);
}


/**
 * Angle is in degree
 */
void GameObject::SetRotation(float angle, glm::vec3 direction)
{
	this->angle = angle;
	this->rotationVector = glm::normalize( direction);

	//const glm::mat4 rotationMatrix = glm::rotate(glm::radians(angle), direction);
	//const glm::vec4 newDir = rotationMatrix * glm::vec4(this->direction, 1.0);
	//const glm::vec3 vnewDir(newDir);
	//this->direction = glm::normalize(vnewDir);
}

void GameObject::SetRotationVector(glm::vec3 rotationVector)
{
	SetRotation(this->angle, rotationVector);
}



void GameObject::Translate(glm::vec3 dir, float speed)
{
	this->position += dir * speed;
}

const glm::vec3& GameObject::GetDirection()
{
	return direction;
}

const glm::vec3& GameObject::GetRotationVector()
{
	return rotationVector;
}

const float& GameObject::GetAngle()
{
	return angle;
}


GameObject::GameObject()
{
	model = glm::mat4(1.0f);
	position = glm::vec3(0.f);
	scale = glm::vec3(1.f);
	rotationVector = glm::vec3(0.f, 1.f, 0.f);
	direction = glm::vec3(0.f, 0.f, 1.f);
	angle = 0;
}

void GameObject::Draw(Shader& shader)
{
	
	model = glm::mat4(1.0f);
	model = glm::scale(model, scale);
	
	model = glm::translate(model, position);


	const glm::mat4 rotationMatrix = glm::rotate(glm::radians(angle), rotationVector);
	const glm::vec4 newDir = rotationMatrix * glm::vec4(glm::vec3(0.f, 0.f, 1.f), 1.0);
	const glm::vec3 vnewDir(newDir);
	this->direction = glm::normalize(vnewDir);
	
	
	float characterAngle = glm::degrees(atan2(direction.x, direction.z));
	characterAngle = ((int)characterAngle + 360) % 360;
	model = glm::rotate(model, glm::radians(characterAngle ), glm::normalize(glm::vec3(0, 1, 0)));
	
	
	//model =  glm::lookAt(this->position, this->direction + this->position	, glm::vec3(0.f, 1.f, 0.f)) * model;
	



	
	shader.SetMat4("model", model);

	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
	shader.SetMat3("normalMatrix",normalMatrix); 
	

}

void GameObject::Update(float deltaTime)
{
	//const glm::mat4 rotationMatrix = glm::rotate(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	//const glm::vec4 newDir = rotationMatrix * glm::vec4(direction, 1.0);
	//const glm::vec3 vnewDir(newDir);
	//direction = vnewDir;

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

ModelGameObject::ModelGameObject(Model* model)
	: model (model)
{
}

void ModelGameObject::Draw(Shader& shader)
{
	GameObject::Draw(shader);
	model->Draw(shader);
}

SkeletalModelGameObject::SkeletalModelGameObject(SkeletalModel* skeletalModel)
	: skeletalModel(skeletalModel)
{
}
