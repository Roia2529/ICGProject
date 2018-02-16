#pragma once
#pragma once
#ifndef _PLANE_H_INCLUDED_
#define _PLANE_H_INCLUDED_
#include "../glHeader.h"

class PlaneObj : public cyTriMesh
{

public:
	cy::GLSLProgram glslProgram;

	void init() {

	}

	void textureBind(int mid, int mapid, int bid) {
		//material[mid].textureMap[mapid].gltexture.Bind(bid);
		/*for (unsigned int i = 0; i < NM(); i++) {
		diffuse_Texture.Bind();
		}*/
	}

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");

		//for shading
		glslProgram.RegisterUniform(1, "Tmap_kd");

		CY_GL_REGISTER_DEBUG_CALLBACK;
		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

	int getvArraySize() {
		return 18;
	}

	float* getvArrayPtr() {
		return vertice;
	}


private:
	cy::Point3f *vArrayPtr, *nArrayPtr, *tArrayPtr;
	float vertice[18] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
};

//-------------------------------------------------------------------------------

#endif
