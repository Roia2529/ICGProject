#version 330 core
out vec4 color;

in vec2 posT;

uniform sampler2D Tmap_kd;

void main() {
	color = vec4(texture(Tmap_kd, posT).rgb, 1.0f);
		//color = vec4(1.0f,0.0f,0.0f, 1.0f);
}