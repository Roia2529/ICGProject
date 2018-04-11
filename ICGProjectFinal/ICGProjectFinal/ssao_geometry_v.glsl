#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

//uniform bool invertedNormals;

uniform mat4 viewmodel;
uniform mat3 viewmode_invtrans;
uniform mat4 mat;

void main()
{
	vec4 viewPos = viewmodel * vec4(aPos, 1.0);
	FragPos = viewPos.xyz;

	//Normal = viewmode_invtrans * (invertedNormals ? -aNormal : aNormal);
	Normal = viewmode_invtrans *  aNormal;
	gl_Position = mat * vec4(aPos, 1.0);
}