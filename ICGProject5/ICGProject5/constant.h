#ifndef _CONSTANT_H
#define _CONSTANT_H

class glb {
	public:
	const char*	TITLE = "ICGProject 5";
	const char* VSHADER = "vshader.glsl";
	const char* FSHADER = "fshader.glsl";

	const char* VSHADER_NOTEX = "vshader.notex.glsl";
	const char* FSHADER_NOTEX = "fshader.notex.glsl";
	
	const char* P_VSHADER = "plane_vshader.glsl";
	const char* P_FSHADER = "plane_fshader.glsl";

	const char* MATRIX_NAME = "mat";
	//Screen Constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	const bool USE_TEXTURE = true;

	//const double INIT_SCALE = 0.0007;
	const double INIT_SCALE = 0.05;

	const int COLOR_MODE = 3;
	enum { ROTATE, ZOOMING }	MOVE_MODE;

};
#endif /* constant_h */