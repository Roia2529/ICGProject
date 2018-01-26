#pragma once
#include "glfunc.h"
#include "Object/objects.h"
//#include "math_3d.h"
#pragma once
static int start;
extern cy::TriMesh trimesh;
std::vector<TriObj> objList;

cy::Point3f *Vertices;
GLfloat glvertices[18];
GLuint vao, *VBO;
GLuint *vbid;

bool LoadObj(const char *filename, bool loadMtl) {
	TriObj *tobj = new TriObj;
	if ( ! tobj->Load(filename, loadMtl ==NULL ) ) {
        printf(" -- ERROR: Cannot load file \"%s.\"", filename);
        delete tobj;
        return false;
    } 
    else {
        objList.push_back(*tobj);  // add to the list
    }    
	
	printf("Sucessfully load %s!\n", filename);
	//Vertices = &(trimesh.V(0));
	Vertices = tobj->getVertices();
	//initGLEW();
	return true;
}

bool glinitGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
	if (objList[0].initGLSLProgram(vShaderFile, fShaderFile)) {
		std::cout << "Loading Shaders done." << std::endl;
		return true;
	}
	return false;
}

void glBufferBind() {
	TriObj *tobj = &objList[0];
	//generate vertex array and bind
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//printf("%d vertices found in Vertices\n", tobj->getNumVertex());

	for (int i = 0, j=0; i < 6; i++) {
		glvertices[j++] = Vertices[i].x;
		glvertices[j++] = Vertices[i].y;
		glvertices[j++] = Vertices[i].z;
	}
	
	//test
	vbid = new GLuint[1];
	glGenBuffers(1, vbid);
	glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*tobj->getNumVertices(), tobj->getVertices(), GL_STATIC_DRAW);
	//glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	/*
	VBO = new GLuint[trimesh.NV()];
	glGenBuffers(trimesh.NV(), VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);//
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*3, Vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f) * trimesh.NV(), Vertices, GL_STATIC_DRAW);;
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	*/
}

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

void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y) {

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		}
	}

void GLGetSpecialKey(int key, int x, int y){
	switch (key) {
	case (GLUT_KEY_F6):
		std::cout << "Recompiling Shaders" << std::endl;
		//recompile
		std::cout << "Compilation Done" << std::endl;
		break; 	
	default:
		break;
	}
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
	/*
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
	*/
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
	glBindVertexArray(vao);
	glVertexAttrib3f((GLuint)1, 1.0, 0.0, 0.0);
	glDrawArrays(GL_TRIANGLES, 0, 18);	// draw first object
	//glDrawArrays(GL_TRIANGLES, 0, objList[0].getNumVertex());	// draw first object
	//drwa stuff
	//Render quad
	/*
	glBegin(GL_QUADS);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(-0.5f, 0.5f);
	glEnd();
	*/

	//Update screen
	glutSwapBuffers();
}