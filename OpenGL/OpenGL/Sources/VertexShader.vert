#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 aJointIndices;
layout (location = 6) in vec4 aJointWeights;


out mat3 TBN;
out vec2 TexCoords;
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;


//Animation and skinning

#define MAX_NUM_OF_BONES 200
uniform int NumOfBones;
uniform mat4 JointsTransformation[MAX_NUM_OF_BONES];



void main()
{
	mat4 JointPallet = mat4(1.0);

	vec4 finalVertexPosition = vec4(0);
	for(int i = 0 ; i < 4 ; i++) // loop through the joints that have an effect on this vertex (Max = 4 joints)
	{
		int JointIndex = aJointIndices[i];
		if(JointIndex == -1 )
		{
			continue;
		}

		if(JointIndex >= MAX_NUM_OF_BONES) 
        {
            finalVertexPosition = vec4(aPos,1.0f);
            break;
        }
		float JointWeight = aJointWeights[i];
		finalVertexPosition += JointWeight * (JointsTransformation[JointIndex] * vec4(aPos,1));
	}



	gl_Position = projection * view  * model * finalVertexPosition;
	fragPosition = vec3(model * finalVertexPosition);
	TexCoords = aTexCoords;

	vec3 N = normalize(normalMatrix * aNormal);
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 B = normalize(normalMatrix * aBiTangent);

	TBN = mat3(T, B, N);
}