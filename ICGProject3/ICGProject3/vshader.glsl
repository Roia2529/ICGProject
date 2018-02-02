#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexN;

out vec3 faceN;

uniform mat4 mat;
uniform mat3 matInvTrans;

void main() {
	gl_Position = mat * vec4(position, 1.0f);
	faceN = matInvTrans * vertexN;
}