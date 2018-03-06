#version 330 core
layout(location = 0) in vec3 position;

out vec4 shadowP;
out vec3 faceP;

uniform mat4 mat;
uniform mat3 matOrigin;
uniform mat4 shadowmvp;

void main() {
	gl_Position = mat * vec4(position, 1.0f);
	faceP = matOrigin * position;
	shadowP = shadowmvp * vec4(position, 1.0f);
}