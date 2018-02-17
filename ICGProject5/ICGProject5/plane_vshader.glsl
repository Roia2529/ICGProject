#version 330 core
layout(location = 0) in vec3 position;

out vec2 posT;
uniform mat4 mat;


void main() {
	gl_Position = mat * vec4(position, 1.0f);
	posT = vec2(position.x / 2 + 0.5, position.y / 2 + 0.5);
}