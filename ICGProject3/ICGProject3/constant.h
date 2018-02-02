#ifndef _CONSTANT_H
#define _CONSTANT_H

class glb {
	public:
	const char*	TITLE = "ICGProject 3";
	const char* VSHADER = "vshader.glsl";
	const char* FSHADER = "fshader.glsl";
	const char* MATRIX_NAME = "mat";
	//Screen Constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	const bool USE_ELEMT = true;

	const double INIT_SCALE = 0.05;
	enum { ROTATE, ZOOMING }	MOVE_MODE;

};
#endif /* constant_h */