#version 400 core

layout(location = 0) out vec4 color;
in vec3 faceN;

void main() {
	color = vec4(faceN, 1.0);
}