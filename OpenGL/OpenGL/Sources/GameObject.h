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
	float angle;

public:
	GameObject();
	virtual void Draw(Shader& shader);

	const glm::vec3& GetPosition();
	void SetPosition(glm::vec3 position);
	void SetAngle(float angle);
	void Translate(glm::vec3 dir, float speed);
};

class SkeletalModelGameObject : public GameObject
{
public:
	SkeletalModelGameObject(SkeletalModel* skeletalModel);
	void Draw(Shader& shader)override;
private:
	SkeletalModel* skeletalModel = nullptr;

	
};
