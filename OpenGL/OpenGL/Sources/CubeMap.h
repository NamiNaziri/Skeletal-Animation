#pragma once
#include <string>
#include <vector>

#include "Shader.h"
class CubeMap
{
public:
	CubeMap();
	CubeMap(std::string path);
	void SetCubeMapPath(std::string path);
	void Draw(Shader& shader);
private:
	void LoadCubeMap(std::string path);
	
	void CreateCubeMapObject();
	
	unsigned int textureID;
	unsigned int VAO, VBO;
	
	static const std::string faces[];
	static const std::vector<float> skyboxVertices;
};

