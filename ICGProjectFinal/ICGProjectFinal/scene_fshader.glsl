#version 400 core
layout(location = 0) out vec4 color;

in vec3 faceN;
in vec3 faceP;

void main() {
	//color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	color = vec4(clamp(normalize(faceN), 0.0, 1.0), 1.0f);
}