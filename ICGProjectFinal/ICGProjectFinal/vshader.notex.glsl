#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexN;

out vec3 faceN;
out vec3 faceP;
out vec4 shadowcoord;

uniform mat4 mat;
uniform mat3 matInvTrans;
uniform mat3 matOrigin;
uniform mat4 shadowmvp;

void main() {
	gl_Position = mat * vec4(position, 1.0f);
	faceP = matOrigin * position;
	faceN = matInvTrans * vertexN;
	shadowcoord = shadowmvp * vec4(position, 1.0f);
}