#pragma once
#include "glfunc.h"
#include "Object/objects.h"
#include "Object/plane.h"
#include "Object/light.h"
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

cy::Point3f lightpos(4.0f, 4.0f, 0.0f);
cy::Point3f camera(0.2f, 1.1f, 12.0f);
cy::Point3f cameraFocus(0.0f, 0.0f, 0.0f);
cy::Point3f cameraUpVec(0.0f, 1.0f, 0.0f);
cy::Point3f cameraCurrentPos;
///---object-------//
std::vector<TriObj> objList;
//GLuint vao;
//GLuint *vbid;
///----plane-------//
PlaneObj plane;
//GLuint vao_p;
//GLuint *vbid_p;
///----skybox------//
//CubeMap cubemap;
//GLuint vao_cube;
//GLuint *vbid_cube;
///-----light------//
lightObj lightobj(lightpos);
//------------------------------//
//cy::Matrix4f object_scale;
cy::Matrix4f object_model = cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);

//cy::Matrix4f object_model;
cy::Matrix4f p_scale_Matrix;
cy::Matrix4f pro;
cy::Matrix4f view;
GLfloat aspect = (GLfloat)glbv.SCREEN_WIDTH/ (GLfloat)glbv.SCREEN_HEIGHT;
//------------------------------//
cy::GLRenderTexture<GL_TEXTURE_2D> glrenderT;
//------------------------------//
cy::GLRenderDepth2D shadowMap;
//------------------------------//

	bool initShaders() {
		const char *vshader = glbv.USE_TEXTURE ? glbv.VSHADER : glbv.VSHADER_NOTEX;
		const char *fshader = glbv.USE_TEXTURE ? glbv.FSHADER : glbv.FSHADER_NOTEX;

		//object shader
		std::cout << "Loading Object Shaders....." << std::endl;
		if (!objList[0].initGLSLProgram(vshader, fshader)) {
			std::cout << "Loading Object Shaders failed." << std::endl;
			return false;
		}

		//plane shader
		std::cout << "Loading Plane Shaders....." << std::endl;
		if (!plane.initGLSLProgram(glbv.P_VSHADER, glbv.P_FSHADER)) {
			std::cerr << "load Plane shader failed" << std::endl;
			return false;
		}

		std::cout << "Loading Light Shaders....." << std::endl;
		if (!lightobj.initGLSLProgram("vshader_shadow.glsl", "fshader_shadow.glsl")) {
			std::cerr << "load light shader failed" << std::endl;
			return false;
		}

		//if (!cubemap.initGLSLProgram(glbv.CUBE_VSHADER, glbv.CUBE_FSHADER)) {
		//	std::cerr << "load cube shader failed" << std::endl;
		//	return false;
		//}
		return true;
	}

	bool LoadObj(const char *filename, bool loadMtl) {
		TriObj *tobj = new TriObj(glbv.COLOR_MODE);
		printf("********Loading object....*******\n");
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

		//printf("********Loading cube....*******\n");
		//if (!cubemap.Load("cube.obj", false, false, glbv.faces)) {
		//	printf(" -- ERROR: Cannot load file \"cube.obj.\"");
		//	return false;
		//}
		//printf("Sucessfully load cube.obj!\n");

		printf("********Loading light....*******\n");
		if (!lightobj.Load("light.obj", false, false)) {
			printf(" -- ERROR: Cannot load file \"light.obj.\"");
			return false;
		}
		printf("Sucessfully load light.obj!\n");
		return true;
	}

	void getProjection() {
		static float znear = 0.01f, zfar = 100.0f;
		pro = cy::Matrix4f::MatrixPerspective(60.0f / 180.0f*cy::cyPi<float>(), (float)glbv.SCREEN_WIDTH / (float)glbv.SCREEN_HEIGHT, znear, zfar);
	}

	cy::Matrix4f OrthographicProj(float asp, float x, float y, float n, float f, bool fixaspect = false) {
		if (!fixaspect) {
			if (asp > 1) { x = y * asp; }
			else { y = x / asp; }
		}
		cy::Matrix4f proj;
		proj = cy::Matrix4f::MatrixScale(2.0f / x, 2.0f / y, -2.0f / (f - n));
		proj(2, 3) = -(f + n) / (f - n);
		proj(3, 3) = 1;
		return proj;
	}

	void prepareMatrix(GLfloat scaleinit) {
		scale = scaleinit;
		objList[0].setScale(scale);
		getProjection();

		plane.setScale(glbv.PLANE_SCALE);

		lightobj.setScale(0.1f);
		updateView();
	}

	void updateView() {
		
		cameraCurrentPos = tkball_camera.getMatrix().GetSubMatrix3() * ((cameraFocus - camera) * tkball_camera.getZoomRatio() + camera);
		view = LookAt(cameraCurrentPos,cameraFocus,cameraUpVec);
		//std::cout << "Camera current pos: (" << cameraCurrentPos.x << ", " << cameraCurrentPos.y << ", " << cameraCurrentPos.z << std::endl;

	}
		
	void glBufferBind() {

		//---------Object---------//
		objList[0].initBufferBind();

		//-----------Plane---------//
		plane.initBufferBind();

		//skybox//
		//cubemap.initBufferBind();

		lightobj.initBufferBind();
		lightobj.initBufferBindPlane();
	}
	
	bool initGLRenderTexture(int width, int height) {
		if (glrenderT.Initialize(true, 4, width, height)) {
			
			glrenderT.SetTextureMaxAnisotropy();
			//glrenderT.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
			glrenderT.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);

			//glrenderT.SetTextureWrappingMode(GL_CLAMP,GL_CLAMP);
			return true;
		}
		return false;
	}

	bool initShadowMap(int width, int height) {
		if (shadowMap.Initialize(false, width, height)) {

			//shadowMap.SetTextureMaxAnisotropy();
			//shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
			shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);

			//shadowMap.SetTextureWrappingMode(GL_CLAMP, GL_CLAMP);
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
			initShaders();
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
		shadowMap.Bind();
		//Clear color buffer
		glClearColor(0.3f, 0.6f, 0.3f, 0.f);
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		cy::Point3f newlightpos = cy::Point3f(light.getMatrix()*cy::Point4f(lightpos, 1.0f));
		//std::cout << newlightpos.x << " " << newlightpos.y << " " << newlightpos.z << std::endl;
		cy::Matrix4f lightView, lightProjection;
		lightView = LookAt(newlightpos, cameraFocus, cameraUpVec);
		//lightProjection = OrthographicProj(aspect, 10.0f, 10.0f, -0.5f, 10.f);
		lightProjection = pro;

		//transformation
		cy::Matrix4f M_wo_pro;
		//M_wo_pro = tkball.getMatrix() * object_model * objList[0].getScale();
		M_wo_pro = object_model * objList[0].getScale();

		//draw to shadow map
		{
			//teapot
			objList[0].programBind();
			objList[0].BufferBind();
			objList[0].updateUniform(M_wo_pro, lightProjection, lightView, newlightpos, cameraCurrentPos,
				lightProjection, lightView, 1, 0);
			objList[0].DrawArray();

			//plane
			plane.programBind();
			plane.BufferBind();
			plane.updateupdateUniform(lightProjection, lightView, lightProjection, lightView, newlightpos, 1 ,0);
			plane.DrawArray();

		}
		//unbind framebuffer
		//glrenderT.Unbind();
		////glrenderT.
		shadowMap.Unbind();
		//shadowMap.BuildTextureMipmaps();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			shadowMap.BindTexture(0);
			/*glrenderT.BindTexture(0);
			glrenderT.BuildTextureMipmaps();*/

		/*	lightobj.programBind();
			lightobj.BufferBindPlane();
			lightobj.updateFrameBuffer(0);
			lightobj.DrawArrayPlane();*/

			objList[0].programBind();
			objList[0].BufferBind();
			objList[0].updateUniform(M_wo_pro, pro, view, newlightpos, cameraCurrentPos, lightProjection, lightView, 3,0);
			objList[0].DrawArray();

			//plane
			plane.programBind();
			plane.BufferBind();
			plane.updateupdateUniform(pro, view, lightProjection, lightView, newlightpos, 3, 0);
			plane.DrawArray();

			//draw light
			lightobj.BufferBind();
			lightobj.updateUniform(pro*view*light.getMatrix());
			lightobj.DrawArray();
		}

		//Update screen
		//CY_GL_REGISTER_DEBUG_CALLBACK;
		glutSwapBuffers();
	}
