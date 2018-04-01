#version 330 core
layout(location = 0) out vec4 color;

in vec3 faceN;
in vec3 faceP;
in vec4 shadowcoord;

uniform vec3 Mtl_ka;
uniform vec3 Mtl_kd;
uniform vec3 Mtl_ks;

uniform vec3 cameraPos;
uniform sampler2D shadowmap;
//uniform sampler2DShadow shadowmap;

uniform vec3 lightP;
uniform int colormode;

const vec3 kd = vec3(0.2f, 0.9f, 0.2f);
const vec3 ks = vec3(1.0f, 1.0f, 1.0f);
const float alpha = 5.0;
const float gamma = 1.2;
const vec3 ambientL = vec3(0.2f, 0.2f, 0.2f);
const vec3 specularL = vec3(1.0f, 1.0f, 1.0f);
const vec3 diffuseL = vec3(1.0f, 1.0f, 1.0f);
const float reflect_ratio = 0.6;
const float bias = 0.00005;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowmap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	//vec3 normal = normalize(fs_in.Normal);
	//vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// check whether current frag pos is in shadow
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	// PCF
	//float shadow = 0.0;
	//vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	//for (int x = -1; x <= 1; ++x)
	//{
	//	for (int y = -1; y <= 1; ++y)
	//	{
	//		float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
	//		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	//	}
	//}
	//shadow /= 9.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (projCoords.z > 1.0)
		shadow = 0.0;

	return 1.0 - shadow;
}

void main() {
	if(colormode==1){
		color = vec4(clamp(normalize(faceN),0.0,1.0), 1.0f);
	}
	//else if (colormode == 2) {
	//	vec3 N = normalize(faceN);
	//	vec3 L = normalize(cameraPos - faceP);
	//	vec3 V = normalize(reflect(-L, N));
	//	vec3 H = normalize(L + V);

	//	float costheta = max(0.0f, dot(N, L));
	//	float cosHN = max(0.0f, dot(H, N));

	//	vec3 color_ka = kd;
	//	vec3 color_kd = kd;
	//	vec3 color_ks = ks;

	//	vec3 ambientC = color_ka * ambientL;
	//	vec3 diffuseC = color_kd * diffuseL * costheta;
	//	vec3 specularC = color_ks * specularL * costheta * pow(cosHN, alpha);
	//	color = reflect_ratio * texture(skybox, V) + (1- reflect_ratio)*vec4(ambientC + diffuseC + specularC, 1.0f);

	//	//color = reflect_ratio * texture(skybox, V) + ();
	//}
	else if (colormode == 3) {
		//float inshadow = texture(shadowmap, shadowcoord.xyz / shadowcoord.w);
		float inshadow = ShadowCalculation(shadowcoord);
		/*float inshadow = 1.0;
		if (texture(shadowmap, 0.5 * shadowcoord.xy + 0.5).z  <  (0.5 * shadowcoord.z + 0.5)) {
			inshadow = 0.5;
		}
*/
		vec3 N = normalize(faceN);
		vec3 L = normalize(lightP - faceP);
		vec3 V = normalize(reflect(-L, N));
		vec3 H = normalize(L + V);

		float costheta = max(0.0f, dot(N, L));
		float cosHN = max(0.0f, dot(H, N));

		vec3 color_ka = kd;
		vec3 color_kd = kd;
		vec3 color_ks = ks;

		vec3 ambientC = color_ka * ambientL;
		vec3 diffuseC = color_kd * diffuseL * costheta;
		vec3 specularC = color_ks * specularL * costheta * pow(cosHN, alpha);
		color = vec4(ambientC + inshadow * (diffuseC + specularC), 1.0f);
	}
	else{
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
}