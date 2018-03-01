#version 330 core
layout(location = 0) out vec4 color;

in vec2 posT;

uniform ivec2 center;
uniform sampler2D framebuffer;

void main() {
	ivec2 window = textureSize(framebuffer, 0);
	float u = 1 - (gl_FragCoord.x - center.x) / window.x;
	float v = (gl_FragCoord.x - center.x) / window.x;
	color = texture(framebuffer, vec2(u,v)).rgba;
	//color = vec4(1.0f,0.0f,0.0f, 1.0f);
}