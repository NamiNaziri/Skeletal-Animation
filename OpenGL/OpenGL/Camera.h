#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{

public:
	Camera(glm::vec3 position, glm::vec3 up,float fov = 45.f,float pitch = 0.f ,float yaw = -90);
	void Rotate(float pitch, float yaw);
	void Translate(glm::vec3 direction, float speed);

	
	void SetFOV(float fov);

	
	glm::mat4 GetViewMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetForward();
	glm::vec3 GetRight();

	float GetPitch();
	float GetYaw();
	float GetFOV();

private:
	glm::vec3 position;
	glm::vec3 forwardVector;
	glm::vec3 upVector;

	float pitch;
	float yaw;
	float FOV = 45.f;
};

