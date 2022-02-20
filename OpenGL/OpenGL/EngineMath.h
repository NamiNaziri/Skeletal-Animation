#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace EngineMath
{
	struct SQT // this saves local transformation of a bone in relation to its parents. 
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
	};
}
