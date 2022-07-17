#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{

public:
	Camera(glm::vec3 position, glm::vec3 up,float fov = 45.f,float pitch = 0.f ,float yaw = -90, float speed = 5);
	void Rotate(float pitch, float yaw);
	void Translate(glm::vec3 direction, float speed);
	void SetPosition(glm::vec3 newPosition);
	
	void SetFOV(float fov);
	void SetSpeed(float speed);
	
	glm::mat4 GetViewMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetForward();
	glm::vec3 GetRight();
	glm::vec3 GetUp();

	float GetPitch();
	float GetYaw();
	float GetFOV();
	float GetSpeed();
private:
	glm::vec3 position;
	glm::vec3 forwardVector;
	glm::vec3 upVector;

	float pitch;
	float yaw;
	float FOV = 45.f;
	float speed = 60;
};

