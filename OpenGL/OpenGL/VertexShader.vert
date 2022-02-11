#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;


out mat3 TBN;
out vec2 TexCoords;
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = projection * view * model * vec4(aPos,1);
	fragPosition = vec3(model * vec4(aPos,1));
	TexCoords = aTexCoords;

	vec3 N = normalize(normalMatrix * aNormal);
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 B = normalize(normalMatrix * aBiTangent);

	TBN = mat3(T, B, N);
}