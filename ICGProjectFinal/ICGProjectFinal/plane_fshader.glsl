#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int mode;

void main()
{
	if (mode==1) {
		float col = texture(screenTexture, TexCoords).r;
		FragColor = vec4(col, col, col, 1.0);
	}
	else {
		vec3 col = texture(screenTexture, TexCoords).rgb;
		FragColor = vec4(col, 1.0);
	}

}