#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform vec3 lightPos;
uniform bool noSSAO;
const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
const vec3 ambientL = vec3(0.2f, 0.2f, 0.2f);

//uniform float linearpar;
//uniform float quadratic;
const float linearpar=0.09;
const float quadratic=0.032;

void main()
{
	// retrieve data from gbuffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
	float AmbientOcclusion = texture(ssao, TexCoords).r;

	// Shading
	vec3 ambient = vec3(0.4 * Diffuse * AmbientOcclusion);
	vec3 lighting = ambient;
	if (noSSAO) {
		lighting = Diffuse*0.4; //constant ambient light for all the scene
	}
	vec3 viewDir = normalize(-FragPos); 
										
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
	vec3 specular = lightColor * spec;
	// attenuation
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (1.0 + linearpar * distance + quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;
	lighting += diffuse + specular;
	FragColor = vec4(lighting, 1.0);
}