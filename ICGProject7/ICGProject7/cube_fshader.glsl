#version 400 core
layout(location = 0) out vec4 fragcolor;

in vec3 posT;

uniform samplerCube skybox;

void main() {
	fragcolor = texture(skybox, posT).rgba;
	//fragcolor = vec4(1.0f, 0.0f, 0.0f ,1.0f);
}