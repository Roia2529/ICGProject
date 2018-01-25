#version 330 core

layout(location = 0) in vec3 position;
uniform matrix4 mat;

void main() {
	gl_Position = vec4(position, 1.0f);
	ftexcoord = position * 0.5 + 0.5;
}

