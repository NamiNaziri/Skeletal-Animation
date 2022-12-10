#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{

public:

	// program ID
	unsigned int ID;
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	void SetShaderPath(const char* vertexPath, const char* fragmentPath);

	

	void use();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetMat3(const std::string& name, glm::mat3 value) const;
	void SetMat4(const std::string& name, glm::mat4 value) const;
	void SetVec3(const std::string& name, glm::vec3 value) const;
private:
	void CheckCompileErrors(unsigned int shader, std::string type);
	void GenerateShader(const char* vertexPath, const char* fragmentPath);
};

