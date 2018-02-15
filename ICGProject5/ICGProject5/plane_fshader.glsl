#version 330 core
out vec4 color;

in vec3 faceN;
in vec3 faceP;
in vec2 posT;

uniform sampler2D Tmap_ka;
uniform sampler2D Tmap_kd;
uniform sampler2D Tmap_ks;

uniform bool Use_Tmap_ka;
uniform bool Use_Tmap_kd;
uniform bool Use_Tmap_ks;

uniform vec3 Mtl_ka;
uniform vec3 Mtl_kd;
uniform vec3 Mtl_ks;

uniform vec3 lightP;
uniform int colormode;

const vec3 kd = vec3(0.8f, 0.2f, 0.2f);
const vec3 ks = vec3(1.0f, 1.0f, 1.0f);
const float alpha = 5.0;
const float gamma = 1.2;
const vec3 ambientL = vec3(0.2f, 0.2f, 0.2f);
const vec3 specularL = vec3(1.0f, 1.0f, 1.0f);
const vec3 diffuseL = vec3(1.0f, 1.0f, 1.0f);

void main() {
	
		color = vec4( texture(Tmap_kd, posT).rgb, 1.0f);
}