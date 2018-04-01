#version 400 core
out vec4 color;
in vec3 ftexcoord;
uniform sampler2D tex;

float near_plane = 0.01f;
float far_plane = 100.0f;
// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
	float depthValue = texture(tex, ftexcoord.xy).r;
	color = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
	//color = vec4(vec3(depthValue), 1.0); 

	//color = vec4(texture(tex, ftexcoord.xy).rgb, 1.0f);
}
