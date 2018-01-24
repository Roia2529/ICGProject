#include "glfunc.h"
#include "constant.h"
cy::TriMesh trimesh;

int main(int argc, char* args[])
{
	printf("%s\n",args[1]);
	LoadObj(args[1],false);
	//Initialize FreeGLUT
	glutInit(&argc, args);

	//Create OpenGL 2.1 context
	glutInitContextVersion(2, 1);

	//Create Double Buffered Window
	//Option: GLUT_RGBA, GLUT_DEPTH, GLUT_DOUBLE
	//GLUT_DOUBLE use 2 buffer, one is saved for new drawing, and it replace the former one when change is done.
	glutInitDisplayMode(GLUT_DOUBLE);

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Project 2");

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	//Do post window/context creation initialization
	if (!initGL())
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}
	/*initialize glew*/
	initGLEW();


	//Set rendering function
	glutDisplayFunc(GLrender); //put rendering func
	glutKeyboardFunc(GLkeyboardInput);

	glutIdleFunc(GLidle);

	//Set main loop
	//glutTimerFunc( 1000 / SCREEN_FPS, runMainLoop, 0 );
	
	//Start GLUT main loop
	//This runs freeGLUT's internal main loop not our main loop.
	glutMainLoop();

	return 0;
}