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
	if(colormode==1){
		color = vec4(clamp(normalize(faceN),0.0,1.0), 1.0f);
	}
	else if (colormode == 3) {
		vec3 N = normalize(faceN);
		vec3 L = normalize(lightP - faceP);
		vec3 V = normalize(reflect(-L, N));
		vec3 H = normalize(L + V);

		float costheta = max(0.0f, dot(N, L));
		float cosHN = max(0.0f, dot(H, N));

		vec3 color_ka = Use_Tmap_ka? texture(Tmap_ka, posT).rgb : Mtl_ka;
		vec3 color_kd = Use_Tmap_kd? texture(Tmap_kd, posT).rgb : Mtl_kd;
		vec3 color_ks = Use_Tmap_ks? texture(Tmap_ks, posT).rgb : Mtl_ks;

		vec3 ambientC = color_ka * ambientL;
		vec3 diffuseC = color_kd * diffuseL * costheta;
		vec3 specularC = color_ks * specularL * costheta * pow(cosHN, alpha);
		color = vec4(ambientC + diffuseC + specularC, 1.0f);
	}
	else{
		color = vec4( texture(Tmap_kd, posT).rgb, 1.0f);
	}
}