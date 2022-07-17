#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>
#include "SkeletalModel.h"
class GameObject
{
protected:
	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotationVector;
	glm::vec3 direction;
	float angle;

public:
	GameObject();
	virtual void Draw(Shader& shader);
	virtual void Update(float deltaTime = 0.f);

	const glm::vec3& GetPosition();
	void SetPosition(glm::vec3 position);
	void SetScale(glm::vec3 scale);
	void SetAngle(float angle);
	void SetRotation(float angle, glm::vec3 direction);
	void SetRotationVector(glm::vec3 roationVector);
	
	void Translate(glm::vec3 dir, float speed);

	
	const glm::vec3& GetDirection();
	const glm::vec3& GetRotationVector();
	const float& GetAngle();
protected:
	
};

class SkeletalModelGameObject : public GameObject
{
public:
	SkeletalModelGameObject(SkeletalModel* skeletalModel);
	void Draw(Shader& shader)override;
private:
	SkeletalModel* skeletalModel = nullptr;
};


class ModelGameObject : public GameObject
{
public:
	ModelGameObject(Model* model);
	void Draw(Shader& shader)override;
private:
	Model* model = nullptr;
};