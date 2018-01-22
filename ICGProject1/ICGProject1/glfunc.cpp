#include "glfunc.h"
#define INTERVAL_BACKGROUND_CHANGE 1
static int start;


bool initGL()
{
	start = (int)time(NULL);;
	//Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);//black

									 //Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		return false;
	}

	return true;
}

void GLkeyboardInput(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC
		glutLeaveMainLoop();
		exit(0);
		break;
	default:
		break;
	}
}

void GLidle() {
	int end = (int)time(NULL);
	if (end - start>INTERVAL_BACKGROUND_CHANGE) {
		//change background color
		//printf("Time [ %d, %d ]\n", start, end );
		start = end;
		float r = float(rand()) / RAND_MAX;
		float g = float(rand()) / RAND_MAX;
		float b = float(rand()) / RAND_MAX;
		glClearColor(r, g, b, 1.f);
	}
	glutPostRedisplay();
}

void GLupdate()
{

}

//never call it directly
void GLrender()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	//drwa stuff
	//Render quad
	glBegin(GL_QUADS);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(-0.5f, 0.5f);
	glEnd();

	//Update screen
	glutSwapBuffers();
}