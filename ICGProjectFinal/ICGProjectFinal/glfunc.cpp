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
cy::Point3f camera(-7.8f, 6.1f, -4.0f);
cy::Point3f cameraFocus(0.0f, 0.0f, 0.0f);
cy::Point3f cameraUpVec(0.0f, 1.0f, 0.0f);
cy::Point3f cameraCurrentPos;
///---object-------//
std::vector<TriObj> objList;
///----plane-------//
PlaneObj plane;
///----skybox------//
CubeMap cubemap;
///-----light------//
lightObj lightobj(lightpos);
//------------------------------//
//cy::Matrix4f object_scale;
cy::Matrix4f object_rotate = cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);

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

//GLSL
cy::GLSLProgram GeometryPass, LightingPass, SSAO, SSAOBlur, viewbuffer;
//Frame buffer for GeometryPas
unsigned int gBuffer;
unsigned int gPosition, gNormal, gAlbedo;
unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
unsigned int rboDepth;

//Frame buffer for SSAO
unsigned int ssaoFBO, ssaoBlurFBO;
unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

//Sample Kernal
std::vector<cy::Point3f> ssaoKernel;
std::vector<cy::Point3f> ssaoNoise;
unsigned int noiseTexture;

	bool LoadObj(const char *filename, bool loadMtl) {
		TriObj *tobj = new TriObj(glbv.COLOR_MODE);
		printf("********Loading object....*******\n");
		//if (!tobj->Load(filename, loadMtl, glbv.USE_TEXTURE)) {
		if (!tobj->Load("nanosuit.obj", loadMtl,glbv.USE_TEXTURE)) {
			printf(" -- ERROR: Cannot load file \"%s.\"", filename);
			delete tobj;
			return false;
		}
		else {
			objList.push_back(*tobj);  // add to the list
		}

		printf("Sucessfully load %s!\n", filename);
		printf("%d object in object list!\n", objList.size());

		printf("********Loading cube....*******\n");
		if (!cubemap.Load("cube.obj", false, false, glbv.faces)) {
			printf(" -- ERROR: Cannot load file \"cube.obj.\"");
			return false;
		}
		printf("Sucessfully load cube.obj!\n");

		/*printf("********Loading light....*******\n");
		if (!lightobj.Load("light.obj", false, false)) {
			printf(" -- ERROR: Cannot load file \"light.obj.\"");
			return false;
		}
		printf("Sucessfully load light.obj!\n");*/
		return true;
	}

	void glBufferBind() {

		//---------Object---------//
		objList[0].initBufferBind();

		//-----------Plane---------//
		plane.initBufferBind();

		//skybox//
		cubemap.initBufferBind();

		//lightobj.initBufferBind();
		//lightobj.initBufferBindPlane();
	}

	bool initShaders() {
		const char *vshader = glbv.USE_TEXTURE ? glbv.VSHADER : glbv.VSHADER_NOTEX;
		const char *fshader = glbv.USE_TEXTURE ? glbv.FSHADER : glbv.FSHADER_NOTEX;

		//object shader
		std::cout << "Loading Object Shaders....." << std::endl;
		//if (!objList[0].initGLSLProgram(vshader, fshader)) {
		if (!objList[0].initGLSLProgram("scene_vshader.glsl", "scene_fshader.glsl")) {
			std::cout << "Loading Object Shaders failed." << std::endl;
			return false;
		}

		//plane shader
		//std::cout << "Loading Plane Shaders....." << std::endl;
		//if (!plane.initGLSLProgram(glbv.P_VSHADER, glbv.P_FSHADER)) {
		//	std::cerr << "load Plane shader failed" << std::endl;
		//	return false;
		//}

		/*std::cout << "Loading Light Shaders....." << std::endl;
		if (!lightobj.initGLSLProgram("vshader_shadow.glsl", "fshader_shadow.glsl")) {
			std::cerr << "load light shader failed" << std::endl;
			return false;
		}*/

		if (!cubemap.initGLSLProgram("scene_vshader.glsl", "scene_fshader.glsl")) {
			std::cerr << "load cube shader failed" << std::endl;
			return false;
		}
		//GeometryPass, LightingPass, SSAO, SSAOBlur;
		//if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		if (!GeometryPass.BuildFiles("ssao_geometry_v.glsl","ssao_geometry_f.glsl")) return false;
		if (!LightingPass.BuildFiles("ssao_v.glsl", "ssao_lighting_f.glsl")) return false;
		if (!SSAO.BuildFiles("ssao_v.glsl", "ssao_f.glsl")) return false;
		if (!SSAOBlur.BuildFiles("ssao_v.glsl", "ssao_blur_f.glsl")) return false;

		if (!viewbuffer.BuildFiles("plane_vshader.glsl", "plane_fshader.glsl")) return false;
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

		//cubemap.setScale(1.0);
		//plane.setScale(glbv.PLANE_SCALE);
		//lightobj.setScale(0.1f);
		updateView();
	}

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	void generateSampleKernel() {
		
		for (unsigned int i = 0; i < 64; ++i)
		{
			float r_x = rand() / (float)RAND_MAX;
			float r_y = rand() / (float)RAND_MAX;
			float r_z = rand() / (float)RAND_MAX;
			cy::Point3f sample(r_x * 2.0 - 1.0, r_y * 2.0 - 1.0, r_z);
			sample.Normalize();
			sample *= rand() / (float)RAND_MAX;
			float scale = float(i) / 64.0;

			// scale samples s.t. they're more aligned to center of kernel
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		for (unsigned int i = 0; i < 16; i++)
		{
			float r_x = rand() / (float)RAND_MAX;
			float r_y = rand() / (float)RAND_MAX;
			cy::Point3f noise(r_x * 2.0 - 1.0, r_y * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise);
		}

		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void updateView() {
		
		cameraCurrentPos = tkball_camera.getMatrix().GetSubMatrix3() * ((cameraFocus - camera) * tkball_camera.getZoomRatio() + camera);
		view = LookAt(cameraCurrentPos,cameraFocus,cameraUpVec);
		std::cout << "Camera current pos: (" << cameraCurrentPos.x << ", " << cameraCurrentPos.y << ", " << cameraCurrentPos.z << std::endl;

	}

	void initGBuffer() {
		const unsigned int SCR_WIDTH = glbv.SCREEN_WIDTH;
		const unsigned int SCR_HEIGHT = glbv.SCREEN_HEIGHT;

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		// color + specular color buffer
		glGenTextures(1, &gAlbedo);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		
		glDrawBuffers(3, attachments);
		// create and attach depth buffer (renderbuffer)
		
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void initSSAOBuffer() {
		const unsigned int SCR_WIDTH = glbv.SCREEN_WIDTH;
		const unsigned int SCR_HEIGHT = glbv.SCREEN_HEIGHT;

		glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Framebuffer not complete!" << std::endl;
		// and blur stage
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glGenTextures(1, &ssaoColorBufferBlur);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	//event handler
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
		cy::Point3f newlightpos = cy::Point3f(light.getMatrix()*cy::Point4f(lightpos, 1.0f));
		//std::cout << newlightpos.x << " " << newlightpos.y << " " << newlightpos.z << std::endl;
		cy::Matrix4f lightView, lightProjection;
		lightView = LookAt(newlightpos, cameraFocus, cameraUpVec);
		lightProjection = pro;

		//transformation
		cy::Matrix4f obmodel;
		obmodel = object_rotate * objList[0].getScale();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Geometry pass
		{
			/*
			uniform bool invertedNormals;
			uniform mat4 viewmodel;
			uniform mat3 viewmode_invtrans;
			uniform mat4 mat;
			*/
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GeometryPass.Bind();
			// room cube
			cy::Matrix4f cubemodel = cubemap.getScale();
			cy::Matrix4f viewmodel = view * cubemodel;
			cy::Matrix4f mat = pro * viewmodel;
			GeometryPass.SetUniform("viewmodel", viewmodel);
			GeometryPass.SetUniform("viewmode_invtrans", viewmodel.GetSubMatrix3().GetTranspose().GetInverse()); // invert normals as we're inside the cube
			GeometryPass.SetUniform("mat", mat);
			//GeometryPass.SetUniform("invertedNormals", 1);
			cubemap.BufferBind();
			cubemap.DrawArray();
			// nanosuit model on the floor
			GeometryPass.Bind();
			viewmodel = view * obmodel;
			mat = pro * viewmodel;
			GeometryPass.SetUniform("viewmodel", viewmodel);
			GeometryPass.SetUniform("viewmode_invtrans", viewmodel.GetSubMatrix3().GetTranspose().GetInverse()); // invert normals as we're inside the cube
			GeometryPass.SetUniform("mat", mat);
			//GeometryPass.SetUniform("invertedNormals", 0);
			objList[0].BufferBind();
			objList[0].DrawArray();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
									  // clear all relevant buffers
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			viewbuffer.Bind();
			plane.BufferBind();
			glBindTexture(GL_TEXTURE_2D, gNormal);
			plane.DrawArray();


			//camera
			/*objList[0].programBind();
			objList[0].BufferBind();
			objList[0].updateUniform(obmodel, pro, view, newlightpos, cameraCurrentPos, lightProjection, lightView, 3,0);
			objList[0].DrawArray();

			glDepthMask(GL_FALSE);
			cubemap.programBind();
			cubemap.BufferBind();

			cubemap.updateUniform(pro, view);
			cubemap.DrawArray();
			glDepthMask(GL_TRUE);*/

		}

		//Update screen
		CY_GL_REGISTER_DEBUG_CALLBACK;
		glutSwapBuffers();
	}
