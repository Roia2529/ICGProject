#pragma once
#include "glfunc.h"
#include "Object/objects.h"
#include "trackerball.h"
//***********************//
extern glb glbv;
extern GLfloat scale;
//***********************//
trackball tkball;
//***********************//
static int start;
std::vector<TriObj> objList;
///----------------------//
cy::Point3f *Vertices;
GLfloat glvertices[18];
GLuint vao, *VBO;
GLuint *vbid;
//------------------------------//
cy::Matrix4f scale_Matrix;
cy::Matrix4f pro;
//------------------------------//

	bool LoadObj(const char *filename, bool loadMtl) {
		TriObj *tobj = new TriObj;
		if (!tobj->Load(filename, loadMtl == NULL)) {
			printf(" -- ERROR: Cannot load file \"%s.\"", filename);
			delete tobj;
			return false;
		}
		else {
			objList.push_back(*tobj);  // add to the list
		}

		printf("Sucessfully load %s!\n", filename);
		printf("%d object in object list!\n", objList.size());
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
	void getProjection() {
		static float znear = 0.0f, zfar = 1000.0f;
		pro = cy::Matrix4f::MatrixPerspective(60.0f / 180.0f*cy::cyPi<float>(), (float)glbv.SCREEN_WIDTH / (float)glbv.SCREEN_HEIGHT, znear, zfar);
	}

	void prepareMatrix(GLfloat scaleinit) {
		scale = scaleinit;
		scale_Matrix.SetScale(scale);

		//put teapot in the center
		scale_Matrix.AddTrans((objList[0].GetBoundMin() + objList[0].GetBoundMax())*scale*-0.5);
		getProjection();
	}

	void glBufferBind() {
		TriObj *tobj = &objList[0];
		//generate vertex array and bind
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		//test
		vbid = new GLuint[1];
		glGenBuffers(1, vbid);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*tobj->getNumVertices(), tobj->getVertices(), GL_STATIC_DRAW);

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


										 //Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			printf("Error initializing OpenGL! %s\n", gluErrorString(error));
			return false;
		}

		//Initialize clear color
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClearColor(glbv.BACKGROUND[0], glbv.BACKGROUND[1], glbv.BACKGROUND[2], glbv.BACKGROUND[3]);

		return true;
	}

	void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y) {

		cy::Point2f pos;
		tkball.mpos2window(x,y,(float)glbv.SCREEN_WIDTH, (float)glbv.SCREEN_HEIGHT,pos);
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ROTATE;
			tkball.saveBeginRotate(pos);
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ZOOMING;
			tkball.saveBeginZoom(pos);
		}
	}

	void GLmouseMotion(GLint x, GLint y) {
		cy::Point2f pos;
		tkball.mpos2window(x, y, glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT, pos);
		switch (glbv.MOVE_MODE)
		{
		case glbv.ROTATE:
			tkball.calculateRotate(pos);
			break;
		case glbv.ZOOMING:
			tkball.calculateZoom(pos);
			break;
		default:
			break;
		}

	}

	void GLGetSpecialKey(int key, int x, int y) {
		switch (key) {
		case (GLUT_KEY_F6):
			std::cout << "Recompiling Shaders" << std::endl;
			//recompile
			glinitGLSLProgram(glbv.VSHADER, glbv.FSHADER);
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
		if (end - start>1) {
			//change background color
			//printf("Time [ %d, %d ]\n", start, end );
			start = end;
			float r = float(rand()) / RAND_MAX;
			float g = float(rand()) / RAND_MAX;
			float b = float(rand()) / RAND_MAX;
			glClearColor(r, g, b, 1.f);
		}
		*/
		
		//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glutPostRedisplay();
	}
	


	//never call it directly
	void GLrender()
	{
		//Clear color buffer
		//glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 1.f);//black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Bind
		objList[0].programBind();
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);

		//transformation
		cy::Matrix4f finalM;
		finalM = pro* tkball.getMatrix() * scale_Matrix;
		//finalM =  tkball.getMatrix() * scale_Matrix;
		objList[0].glslProgram.SetUniform(0, finalM);
		glDrawArrays(GL_POINTS, 0, objList[0].getNumVertices());	// draw first object

		//end
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		glUseProgram(0);

		//Update screen
		glutSwapBuffers();
	}
