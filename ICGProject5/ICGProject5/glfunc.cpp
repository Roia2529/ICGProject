#pragma once
#include "glfunc.h"
#include "Object/objects.h"
#include "Object/plane.h"
#include "trackerball.h"
//***********************//
extern glb glbv;
extern GLfloat scale;
//***********************//
trackball tkball;
trackball light;
trackball tkball_plane;
//***********************//
static int start;

///---object-------//
std::vector<TriObj> objList;
GLuint vao;
GLuint *vbid;
///---plane-------//
PlaneObj plane;
GLuint vao_p;
GLuint *vbid_p;
//------------------------------//
cy::Matrix4f scale_Matrix;
cy::Matrix4f p_scale_Matrix;
cy::Matrix4f pro;
//------------------------------//
cy::GLRenderTexture<GL_TEXTURE_2D> glrenderT;
//cy::GLRenderBuffer<GL_TEXTURE_2D> glrenderT;
//------------------------------//

	bool LoadObj(const char *filename, bool loadMtl) {
		TriObj *tobj = new TriObj;
		if (!tobj->Load(filename, loadMtl,glbv.USE_TEXTURE)) {
			printf(" -- ERROR: Cannot load file \"%s.\"", filename);
			delete tobj;
			return false;
		}
		else {
			objList.push_back(*tobj);  // add to the list
		}

		printf("Sucessfully load %s!\n", filename);
		printf("%d object in object list!\n", objList.size());

		return true;
	}

	bool glinitRT_GLSLProgram(const char *vShaderFile, const char *fShaderFile) {

		if (plane.initGLSLProgram(vShaderFile, fShaderFile)) {
			std::cout << "Loading Plane Shaders done." << std::endl;
			return true;
		}
		return false;
	}

	bool glinitGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (objList[0].initGLSLProgram(vShaderFile, fShaderFile)) {
			std::cout << "Loading Shaders done." << std::endl;
			return true;
		}
		return false;
	}

	void getProjection() {
		static float znear = 0.01f, zfar = 1000.0f;
		pro = cy::Matrix4f::MatrixPerspective(60.0f / 180.0f*cy::cyPi<float>(), (float)glbv.SCREEN_WIDTH / (float)glbv.SCREEN_HEIGHT, znear, zfar);
	}

	void prepareMatrix(GLfloat scaleinit) {
		scale = scaleinit;
		scale_Matrix.SetScale(scale);

		//put teapot in the center
		scale_Matrix.AddTrans((objList[0].GetBoundMin() + objList[0].GetBoundMax())*scale*(-0.5));
		getProjection();

		p_scale_Matrix.SetScale(0.8);

	}

	void glBufferBind() {

		//Object
		TriObj *tobj = &objList[0];
		//generate vertex array and bind
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//3 buffer
		vbid = new GLuint[3];
		glGenBuffers(3, vbid);

		//vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*tobj->getvArraySize(), tobj->getvArrayPtr(), GL_STATIC_DRAW);

		//normals
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*tobj->getvArraySize(), tobj->getnArrayPtr(), GL_STATIC_DRAW);

		//texture
		if (glbv.USE_TEXTURE)
		{
			tobj->initTexture();
			glBindBuffer(GL_ARRAY_BUFFER, vbid[2]);

			glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*tobj->getvArraySize(), tobj->gettArrayPtr(), GL_STATIC_DRAW);

		}
		//Plane
		glGenVertexArrays(1, &vao_p);
		glBindVertexArray(vao_p);

		vbid_p = new GLuint[1];
		glGenBuffers(1, vbid_p);

		glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
		//printf("array size: %d",plane.getvArraySize());
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*plane.getvArraySize(), plane.getvArrayPtr(), GL_STATIC_DRAW);

	}
	
	bool initGLRenderTexture(int width, int height) {
		if (glrenderT.Initialize(true, 4, width, height)) {
			
			glrenderT.SetTextureMaxAnisotropy();
			glrenderT.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
			//glrenderT.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);

			glrenderT.SetTextureWrappingMode(GL_CLAMP,GL_CLAMP);
			return true;
		}
		return false;
	}

	bool initGL()
	{
		//start = (int)time(NULL);;
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

		return true;
	}

	void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y) {

		cy::Point2f pos;
		tkball.mpos2window(x,y,(float)glbv.SCREEN_WIDTH, (float)glbv.SCREEN_HEIGHT,pos);
		int modifier = glutGetModifiers();
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ROTATE;
			
			if(modifier==GLUT_ACTIVE_CTRL){
				light.saveBeginRotate(pos);
			}
			else if (modifier == GLUT_ACTIVE_ALT) {
				tkball_plane.saveBeginRotate(pos);
			}
			else{
				
				tkball.saveBeginRotate(pos);
			}
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ZOOMING;
			if (modifier == GLUT_ACTIVE_ALT) {
				tkball_plane.saveBeginZoom(pos);
			}
			else {
				tkball.saveBeginZoom(pos);
			}
		}
	}

	void GLmouseMotion(GLint x, GLint y) {
		cy::Point2f pos;
		tkball.mpos2window(x, y, glbv.SCREEN_WIDTH, glbv.SCREEN_HEIGHT, pos);
		int modifier = glutGetModifiers();
		switch (glbv.MOVE_MODE)
		{
		case glbv.ROTATE:
			if (modifier == GLUT_ACTIVE_CTRL)
				light.calculateRotate(pos);
			else if (modifier == GLUT_ACTIVE_ALT)
				tkball_plane.calculateRotate(pos);
			else
				tkball.calculateRotate(pos);
			break;
		case glbv.ZOOMING:
			if (modifier == GLUT_ACTIVE_ALT)
				tkball_plane.calculateZoom(pos);
			else
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
	

	void GLrender()
	{

		glrenderT.Bind();
		//Clear color buffer
		glClearColor(0.3f, 0.6f, 0.3f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Bind
		{
			objList[0].programBind();
			
			glBindVertexArray(vao);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			
			//Bind buffer before glVertexAttribPointer
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


			if (glbv.USE_TEXTURE)
			{
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, vbid[2]);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
			
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//transformation
		{
			cy::Matrix4f M_wo_pro;
			cy::Matrix3f M_inv_trans;
			cy::Matrix4f finalM;
			M_wo_pro = tkball.getMatrix() * scale_Matrix;
			M_inv_trans = M_wo_pro.GetSubMatrix3().GetTranspose().GetInverse();

			finalM = pro * M_wo_pro;

			cy::Point3f lightpos(0.0, 0.0f, 10.0f);
			cy::Point3f newlightpos = tkball.getMatrix().GetSubMatrix3()*light.getMatrix().GetSubMatrix3()*lightpos;

			for (unsigned int i = 0; i < objList[0].NM(); i++) {
			//for (unsigned int i = 0; i <1; i++) {

				objList[0].glslProgram.SetUniform(0, finalM);
				objList[0].glslProgram.SetUniform(1, M_inv_trans);
				objList[0].glslProgram.SetUniform(2, M_wo_pro.GetSubMatrix3());
				objList[0].glslProgram.SetUniform(3, newlightpos);

				objList[0].glslProgram.SetUniform(4, objList[0].getMtlKa(i));
				objList[0].glslProgram.SetUniform(5, objList[0].getMtlKd(i));
				objList[0].glslProgram.SetUniform(6, objList[0].getMtlKs(i));

				objList[0].glslProgram.SetUniform(7, glbv.COLOR_MODE);


				if (glbv.USE_TEXTURE)
				{
					objList[0].glslProgram.SetUniform(8, 0);
					objList[0].textureBind(i, 0, 0);
					objList[0].glslProgram.SetUniform(9, 1);
					objList[0].textureBind(i, 1, 1);
					objList[0].glslProgram.SetUniform(10, 2);
					objList[0].textureBind(i, 2, 2);

					objList[0].glslProgram.SetUniform(11, objList[0].hasTextureKa(i));
					objList[0].glslProgram.SetUniform(12, objList[0].hasTextureKd(i));
					objList[0].glslProgram.SetUniform(13, objList[0].hasTextureKs(i));
				}
				
				glDrawArrays(GL_TRIANGLES, objList[0].GetMaterialFirstFace(i) * 3, objList[0].GetMaterialFaceCount(i) * 3);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				if(glbv.USE_TEXTURE)
					glDisableVertexAttribArray(2);
			}
		}

		glrenderT.Unbind();
		//glDisable(GL_DEPTH_TEST);
		//Clear color buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Bind
		{
			plane.programBind();

			glBindVertexArray(vao_p);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glrenderT.BindTexture(0);
			glrenderT.BuildTextureMipmaps();
		}
		
		{
			cy::Matrix4f M_wo_pro;
			cy::Matrix4f finalM;
			M_wo_pro = tkball_plane.getMatrix() * p_scale_Matrix;
			finalM = pro * M_wo_pro;

			plane.glslProgram.SetUniform(0, finalM);
			//std::cout << glrenderT.GetTextureID() << std::endl;
			
			plane.glslProgram.SetUniform(1, 0);
			
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		

		//Update screen
		CY_GL_REGISTER_DEBUG_CALLBACK;
		glutSwapBuffers();
	}
