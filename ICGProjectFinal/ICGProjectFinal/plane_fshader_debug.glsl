#version 330 core
layout(location = 0) out vec4 color;

in vec4 posT;

//uniform ivec2 center;
uniform sampler2DShadow framebuffer;

//const float far_plane = 10.0f;
//const float near_plane = 0.1f;
//// required when using a perspective projection matrix
//float LinearizeDepth(float depth)
//{
//	float z = depth * 2.0 - 1.0; // Back to NDC 
//	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
//}

void main()
{
	float depthValue = texture(framebuffer, posT.xyz/ posT.w);
	// color = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
	color = vec4(vec3(depthValue), 1.0); // orthographic
}