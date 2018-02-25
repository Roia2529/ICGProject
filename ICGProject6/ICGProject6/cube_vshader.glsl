#version 400 core

layout(location = 0) in vec3 position;

out vec3 posT;

uniform mat4 mat;

void main() {
	vec4 pos = mat * vec4(position, 1.0f);
	gl_Position = pos;
	posT = position;
}