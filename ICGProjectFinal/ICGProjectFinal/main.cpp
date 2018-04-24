#include "glfunc.h"
glb glbv;
GLfloat scale;

int main(int argc, char* args[])
{
	printf("%s\n",args[1]);

	//Initialize FreeGLUT
	glutInit(&argc, args);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	
	glutInitWindowSize(glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT);
	glutCreateWindow(glbv.TITLE);
	//glEnable(GL_DEPTH_TEST);

	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

	glEnable(GL_DEPTH_TEST);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	LoadObj(args[1], true);
	glBufferBind();

	std::cerr << "*-----Shaders initialization-----*" << std::endl;
	if (!initShaders()) {
		std::cerr << "Loading shaders failed" << std::endl;
		system("pause");
		return 0;
	}

	
	/*if (!initGLRenderTexture(glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT))
		std::cerr << "Initialize texture buffer failed" << std::endl;

	if (!initShadowMap(glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT))
		std::cerr << "Initialize shadow map failed" << std::endl;
*/
	initGBuffer();
	initSSAOBuffer();
	prepareMatrix(0.7);

	generateSampleKernel();

	glbv.DRAW_MODE = glbv.SSAO;
	/*initialize glew*/
	std::cout <<"---------Instruction-----------" << std::endl;
	std::cout << "ESC : exit" << std::endl;
	std::cout << "left mouse : adjust camera angles" << std::endl;
	std::cout << "right mouse : adjust camera distance" << std::endl;
	std::cout << "ALT : Press ALT to rotate light position by mouse" << std::endl;
	std::cout << "F6 : recompile shader" << std::endl;
	std::cout << "Up/Down : Increase/Decrease radius of SSAO" << std::endl;
	std::cout << "Right/Left : Increase/Decrease bias of SSAO" << std::endl;
	std::cout << "F1: Change mode for drawing" << std::endl;
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

	glutMainLoop();

	return EXIT_SUCCESS;
}