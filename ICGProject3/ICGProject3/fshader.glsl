#version 330 core
out vec4 color;

in vec3 faceN;

void main() {
	color = vec4(normalize(faceN), 1.0f);
}