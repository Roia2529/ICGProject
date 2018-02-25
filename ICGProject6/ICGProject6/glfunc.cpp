#pragma once
#include "glfunc.h"
#include "Object/objects.h"
#include "Object/plane.h"
#include "environment\cubemap.h"
#include "trackerball.h"
//***********************//
extern glb glbv;
extern GLfloat scale;
//***********************//
trackball tkball;
trackball light;
trackball tkball_camera;
//***********************//
static int start;

cy::Point4f lightpos(0.0f, 10.0f, 0.0f, 0.0f);
cy::Point3f camera(2.0f, 5.0f, 0.0f);
cy::Point3f cameraFocus(0.0f, 0.0f, 0.0f);
cy::Point3f cameraUpVec(0.0f, 1.0f, 0.0f);
cy::Point3f cameraCurrentPos;
///---object-------//
std::vector<TriObj> objList;
GLuint vao;
GLuint *vbid;
///----plane-------//
PlaneObj plane;
GLuint vao_p;
GLuint *vbid_p;
///----skybox------//
CubeMap cubemap;
GLuint vao_cube;
GLuint *vbid_cube;
//------------------------------//
cy::Matrix4f object_scale;
cy::Matrix4f object_model = cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);
cy::Matrix4f p_scale_Matrix;
cy::Matrix4f pro;
cy::Matrix4f view;
//------------------------------//
cy::GLRenderTexture<GL_TEXTURE_2D> glrenderT;
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

		if (!cubemap.Load("cube.obj", false, false, glbv.faces)) {
			printf(" -- ERROR: Cannot load file \"cube.obj.\"");
			return false;
		}
		printf("Sucessfully load cube.obj!\n");
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
		static float znear = 0.01f, zfar = 100.0f;
		pro = cy::Matrix4f::MatrixPerspective(60.0f / 180.0f*cy::cyPi<float>(), (float)glbv.SCREEN_WIDTH / (float)glbv.SCREEN_HEIGHT, znear, zfar);
	}

	void prepareMatrix(GLfloat scaleinit) {
		scale = scaleinit;
		object_scale.SetScale(scale);

		//put teapot in the center
		object_scale.AddTrans((objList[0].GetBoundMin() + objList[0].GetBoundMax())*scale*(-0.5));
		getProjection();

		p_scale_Matrix.SetScale(0.8);
		updateView();
	}

	void updateView() {
		
		cameraCurrentPos = tkball_camera.getMatrix().GetSubMatrix3() * ((cameraFocus - camera) * tkball_camera.getZoomRatio() + camera);
		view = LookAt(cameraCurrentPos,cameraFocus,cameraUpVec);
		//std::cout << "Camera current pos: (" << cameraCurrentPos.x << ", " << cameraCurrentPos.y << ", " << cameraCurrentPos.z << std::endl;

	}
		void glBufferBind() {

		//-----------------Object----------------//
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

		//-----------Plane---------//
		//glGenVertexArrays(1, &vao_p);
		//glBindVertexArray(vao_p);

		//vbid_p = new GLuint[1];
		//glGenBuffers(1, vbid_p);

		//glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*plane.getvArraySize(), plane.getvArrayPtr(), GL_STATIC_DRAW);


		//skybox//
		cubemap.initBufferBind();
		//glGenVertexArrays(1, &vao_cube);
		//glBindVertexArray(vao_cube);

		//vbid_cube = new GLuint[1];
		//glGenBuffers(1, vbid_cube);

		//glBindBuffer(GL_ARRAY_BUFFER, vbid_cube[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*cubemap.getvArraySize(), cubemap.getvArrayPtr(), GL_STATIC_DRAW);

		if (!cubemap.initGLSLProgram(glbv.CUBE_VSHADER, glbv.CUBE_FSHADER)) {
			std::cerr << "load cube shader failed" << std::endl;
		}
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

	void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y) {

		cy::Point2f pos;
		tkball.mpos2window(x,y,(float)glbv.SCREEN_WIDTH, (float)glbv.SCREEN_HEIGHT,pos);
		int modifier = glutGetModifiers();
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ROTATE;
			
			if(modifier==GLUT_ACTIVE_CTRL){
				tkball.saveBeginRotate(pos);
			}
			else if (modifier == GLUT_ACTIVE_ALT) {
				light.saveBeginRotate(pos);
			}
			else{
				tkball_camera.saveBeginRotate(pos);
				
			}
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			glbv.MOVE_MODE = glbv.ZOOMING;
			tkball_camera.saveBeginZoom(pos);
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
				tkball.calculateRotate(pos);
			else if (modifier == GLUT_ACTIVE_ALT)
				light.calculateRotate(pos);
			else {
				tkball_camera.calculateRotate(pos);
				updateView();
			}
			break;
		case glbv.ZOOMING:
				tkball_camera.calculateZoom(pos);
				updateView();
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
		
		glutPostRedisplay();
	}
	
	cy::Matrix4f LookAt(cy::Point3f campos, cy::Point3f focus, cy::Point3f up)
	{
		cy::Matrix4f view;
		view.SetView(campos, focus, up);
		return view;
	}

	void GLrender()
	{

		//glrenderT.Bind();
		//Clear color buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//Bind
		//{
		//	objList[0].programBind();
		//	
		//	glBindVertexArray(vao);

		//	glEnableVertexAttribArray(0);
		//	glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//	
		//	//Bind buffer before glVertexAttribPointer
		//	glEnableVertexAttribArray(1);
		//	glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


		//	if (glbv.USE_TEXTURE)
		//	{
		//		glEnableVertexAttribArray(2);
		//		glBindBuffer(GL_ARRAY_BUFFER, vbid[2]);
		//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//	}
		//	
		//}

		//
		////transformation
		//
		//	cy::Matrix4f M_wo_pro;
		//	cy::Matrix3f M_inv_trans;
		//	cy::Matrix4f finalM;
		//	M_wo_pro = view * tkball.getMatrix() * object_model * object_scale;
		//	M_inv_trans = M_wo_pro.GetSubMatrix3().GetTranspose().GetInverse();

		//	finalM = pro * M_wo_pro;
		//{
		//	
		//	cy::Point3f newlightpos = cy::Point3f(view *light.getMatrix()*lightpos);

		//	for (unsigned int i = 0; i < objList[0].NM(); i++) {
		//	//for (unsigned int i = 0; i <1; i++) {

		//		objList[0].glslProgram.SetUniform(0, finalM);
		//		objList[0].glslProgram.SetUniform(1, M_inv_trans);
		//		objList[0].glslProgram.SetUniform(2, M_wo_pro.GetSubMatrix3());
		//		objList[0].glslProgram.SetUniform(3, newlightpos);

		//		objList[0].glslProgram.SetUniform(4, objList[0].getMtlKa(i));
		//		objList[0].glslProgram.SetUniform(5, objList[0].getMtlKd(i));
		//		objList[0].glslProgram.SetUniform(6, objList[0].getMtlKs(i));

		//		objList[0].glslProgram.SetUniform(7, glbv.COLOR_MODE);


		//		if (glbv.USE_TEXTURE)
		//		{
		//			objList[0].glslProgram.SetUniform(8, 0);
		//			objList[0].textureBind(i, 0, 0);
		//			objList[0].glslProgram.SetUniform(9, 1);
		//			objList[0].textureBind(i, 1, 1);
		//			objList[0].glslProgram.SetUniform(10, 2);
		//			objList[0].textureBind(i, 2, 2);

		//			objList[0].glslProgram.SetUniform(11, objList[0].hasTextureKa(i));
		//			objList[0].glslProgram.SetUniform(12, objList[0].hasTextureKd(i));
		//			objList[0].glslProgram.SetUniform(13, objList[0].hasTextureKs(i));
		//		}
		//		
		//		glDrawArrays(GL_TRIANGLES, objList[0].GetMaterialFirstFace(i) * 3, objList[0].GetMaterialFaceCount(i) * 3);

		//		glDisableVertexAttribArray(0);
		//		glDisableVertexAttribArray(1);
		//		if(glbv.USE_TEXTURE)
		//			glDisableVertexAttribArray(2);
		//	}
		//}
		//glBindVertexArray(0);

		glDepthMask(GL_FALSE); 
		//glDepthFunc(GL_LEQUAL);
		cubemap.programBind();
		cubemap.BufferBind();

		cubemap.updateUniform(pro, view);
		cubemap.DrawArray();
		//glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);


		//glrenderT.Unbind();
		//glDisable(GL_DEPTH_TEST);
		//Clear color buffer
		//glClearColor(0.0f, 0.0f, 0.0f, 0.f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		////Bind
		//{
		//	plane.programBind();

		//	glBindVertexArray(vao_p);
		//	glEnableVertexAttribArray(0);
		//	glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//	glrenderT.BindTexture(0);
		//	glrenderT.BuildTextureMipmaps();
		//}
		//
		//{
		//	cy::Matrix4f M_wo_pro;
		//	cy::Matrix4f finalM;
		//	M_wo_pro = tkball_plane.getMatrix() * p_scale_Matrix;
		//	finalM = pro * M_wo_pro;

		//	plane.glslProgram.SetUniform(0, finalM);
		//	//std::cout << glrenderT.GetTextureID() << std::endl;
		//	
		//	plane.glslProgram.SetUniform(1, 0);
		//	
		//}
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(0);
		

		//Update screen
		CY_GL_REGISTER_DEBUG_CALLBACK;
		glutSwapBuffers();
	}
