#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexN;

out vec3 faceN;
out vec3 faceP;

uniform mat4 mat;
uniform mat3 matInvTrans;
uniform mat3 matOrigin;

void main() {
	gl_Position = mat * vec4(position, 1.0f);
	faceP = matOrigin * position;
	faceN = matInvTrans * vertexN;
}