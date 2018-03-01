#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexN;
layout(location = 2) in vec3 positionT;

out vec3 faceN;
out vec3 faceP;
out vec2 posT;

uniform mat4 mat;
uniform mat3 matInvTrans;
uniform mat3 matOrigin;

void main() {
	gl_Position = mat * vec4(position, 1.0f);
	faceP = matOrigin * position;
	faceN = matInvTrans * vertexN;
	posT = vec2(positionT.x, 1 - positionT.y);
}