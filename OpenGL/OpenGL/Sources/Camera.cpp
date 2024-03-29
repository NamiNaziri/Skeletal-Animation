#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float fov, float pitch , float yaw, float speed)
	: position(position), upVector(up),FOV(fov),pitch(pitch),yaw(yaw), forwardVector(glm::vec3(0.0f, 0.0f, -1.0f)), speed(speed)
{
	Rotate(pitch, yaw);
}

void Camera::Rotate(float pitch, float yaw)
{

	this->pitch = pitch;
	this->yaw = yaw;
	
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forwardVector = glm::normalize(direction);
}

void Camera::Translate(glm::vec3 direction, float speed)
{
	position += direction * speed;
}

void Camera::SetPosition(glm::vec3 newPosition)
{
	this->position = newPosition;
}

void Camera::SetFOV(float fov)
{
	FOV = fov;
}

void Camera::SetSpeed(float speed)
{
	this->speed = speed;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + forwardVector, upVector);
}

glm::vec3 Camera::GetPosition()
{
	return position;
}

glm::vec3 Camera::GetForward()
{
	return forwardVector;
}

glm::vec3 Camera::GetRight()
{
	return glm::normalize(glm::cross(forwardVector, upVector));
}

glm::vec3 Camera::GetUp()
{
	return upVector;
}

float Camera::GetPitch()
{
	return pitch;
}

float Camera::GetYaw()
{
	return yaw;
}

float Camera::GetFOV()
{
	return FOV;
}

float Camera::GetSpeed()
{
	return speed;
}
