#include "glfunc.h"
glb glbv;
GLfloat scale;

int main(int argc, char* args[])
{
	printf("%s\n",args[1]);
	//Load teapot
	//LoadObj(args[1],false);
	LoadObj(args[1], true);
	//Initialize FreeGLUT
	glutInit(&argc, args);

	//Create Double Buffered Window
	//Option: GLUT_RGBA, GLUT_DEPTH, GLUT_DOUBLE
	//GLUT_DOUBLE use 2 buffer, one is saved for new drawing, and it replace the former one when change is done.
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	
	glutInitWindowSize(glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT);
	glutCreateWindow(glbv.TITLE);
	//glEnable(GL_DEPTH_TEST);

	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	//glEnable(GL_DEPTH_TEST | GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	//Do post window/context creation initialization
	//
	/*
	if (!initGL())
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}
	*/

	/*initialize glew*/
	std::cout <<"---------Instruction-----------" << std::endl;
	std::cout << "ESC : exit" << std::endl;
	std::cout << "left mouse : adjust angles" << std::endl;
	std::cout << "right mouse : adjust distance" << std::endl;
	std::cout << "CTRL : Press CTRL to move light position by mouse" << std::endl;
	std::cout << "F6 : recompile shader" << std::endl;
	std::cout << "------------------------------" << std::endl;


	//Set rendering function //glDrawArrays
	glutDisplayFunc(GLrender); //put rendering func

	//Set mouse callback
	glutMouseFunc(GLGetMouseButton);
	glutMotionFunc(GLmouseMotion);
	
	//Set keyboard callback
	glutKeyboardFunc(GLkeyboardInput);
	glutSpecialFunc(GLGetSpecialKey);

	glutIdleFunc(GLidle);
	//Start GLUT main loop
	//This runs freeGLUT's internal main loop not our main loop.

	//Bind GLSL Prog and Set uniform var
	if (!glinitGLSLProgram(glbv.VSHADER, glbv.FSHADER)) {
		//CY_GL_REGISTER_DEBUG_CALLBACK;
		std::cerr << "load shader failed" << std::endl;
		system("pause");
		return 0;
	}

	//Bind GLSL Prog and Set uniform var
	if (!glinitGLRenderTexture(glbv.P_VSHADER, glbv.P_FSHADER)) {
		//CY_GL_REGISTER_DEBUG_CALLBACK;
		std::cerr << "load shader failed" << std::endl;
		system("pause");
		return 0;
	}


	//Set Matrix
	prepareMatrix(glbv.INIT_SCALE);

	//Bind vao
	glBufferBind();

	glutMainLoop();

	return EXIT_SUCCESS;
}