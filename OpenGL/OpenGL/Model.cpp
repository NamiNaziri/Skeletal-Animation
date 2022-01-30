#include "Model.h"

Model::Model(std::string path)
{
	//TODO get the file and process it
}

void Model::Draw(Shader& shader)
{
	for (auto mesh : meshes)
	{
		mesh.Draw(shader);
	}
}

