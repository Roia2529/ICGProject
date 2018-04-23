#ifndef _CONSTANT_H
#define _CONSTANT_H

struct Texture {
	cy::GLTexture2<GL_TEXTURE_2D> gltexture;
	unsigned width, height;
	std::vector<unsigned char> texture_data;
};


struct CubeFace {
	unsigned width, height;
	std::vector<unsigned char> texture_data;
};

class glb {
	public:
	const char*	TITLE = "ICGProject Final";
	const char* VSHADER = "vshader.glsl";
	const char* FSHADER = "fshader.glsl";

	const char* VSHADER_NOTEX = "vshader.notex.glsl";
	const char* FSHADER_NOTEX = "fshader.notex.glsl";
	
	const char* P_VSHADER = "plane_vshader.glsl";
	const char* P_FSHADER = "plane_fshader.glsl";

	const char* CUBE_VSHADER = "cube_vshader.glsl";
	const char* CUBE_FSHADER = "cube_fshader.glsl";


	std::vector<std::string> faces
	{
			"cubemap_posx.png",
			"cubemap_negx.png",
			"cubemap_posy.png",
			"cubemap_negy.png",
			"cubemap_posz.png",
			"cubemap_negz.png"
	};

	const char* MATRIX_NAME = "mat";
	//Screen Constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	const bool USE_TEXTURE = false;

	//const double INIT_SCALE = 0.0007;
	const double INIT_SCALE = 0.05;
	const double PLANE_SCALE = 9;

	const int COLOR_MODE = 3;
	enum { ROTATE, ZOOMING }	MOVE_MODE;
	enum { SSAO, SSAOBLUR, SHADING}	DRAW_MODE;

};
#endif /* constant_h */