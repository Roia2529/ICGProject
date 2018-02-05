#version 330 core
out vec4 color;

in vec3 faceN;
in vec3 faceP;

uniform int colormode;
uniform vec3 lightP;

const vec3 kd = vec3(0.8f, 0.2f, 0.2f);
const vec3 ks = vec3(1.0f, 1.0f, 1.0f);
const float alpha = 5.0;
const float gamma = 1.2;
const vec3 ambientL = vec3(0.2f, 0.2f, 0.2f);
const vec3 specularL = vec3(1.0f, 1.0f, 1.0f);
const vec3 diffuseL = vec3(1.0f, 1.0f, 1.0f);

void main() {
	if(colormode==1){
		color = vec4(normalize(faceN), 1.0f);
	}
	else if(colormode==2){
		vec3 N = normalize(faceN);
		vec3 L = normalize(lightP - faceP);
		vec3 V = normalize(reflect(-L,N));
		vec3 H = normalize(L+V);

		float costheta = max(0.0f, dot(N, L));
		float cosHN = max(0.0f, dot(H, N));

		vec3 ambientC =  ambientL * kd;
		vec3 diffuseC = kd * diffuseL * costheta;
		vec3 specularC = ks * specularL * pow(cosHN, alpha);
		color = vec4(ambientC + diffuseC + specularC ,1.0f);
	}
}