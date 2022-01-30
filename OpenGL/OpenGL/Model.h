#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <vector>
class Model
{
public:
	Model(std::string path);

	void Draw(Shader& shader);
	
	
	
private:
	std::vector<Mesh> meshes;

	
};

