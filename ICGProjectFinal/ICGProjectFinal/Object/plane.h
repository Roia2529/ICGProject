#pragma once
#ifndef _PLANE_H_INCLUDED_
#define _PLANE_H_INCLUDED_
#include "BaseObject.h"

class PlaneObj : public BaseObject
{
private:
	unsigned int vbo;
	cy::Matrix4f mvp;
	cy::Point4f pNormal = cy::Point4f(0.0f, 0.0f, 1.0f, 0.0f);

	float vertice[24] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

public:
	
	PlaneObj() : BaseObject() {
		//COLOR_MODE = mode;
	}

	virtual void setScale(GLfloat scaleinit) {
		scale.SetScale(scaleinit);

		scale.AddTrans(-center);

		//model *= cy::Matrix4f::MatrixTrans(cy::Point3f(0, -1.7, 0));
		//model *= cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);
		//model *= cy::Matrix4f::MatrixRotationZ(-cy::cyPi<float>() / 2);
	}


	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");
		glslProgram.RegisterUniform(1, "matOrigin");
		glslProgram.RegisterUniform(2, "shadowmvp");
		glslProgram.RegisterUniform(3, "drawmode");
		glslProgram.RegisterUniform(4, "lightP");
		glslProgram.RegisterUniform(5, "shadowmap");

		//CY_GL_REGISTER_DEBUG_CALLBACK;
		return true;
	}

	void initBufferBind() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//vbid = new GLuint[2];
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*getvArraySize(), &vertice, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	}

	void BufferBind() {
		glBindVertexArray(vao);/*
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);*/

		/*glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		*/	
	}

	void updateupdateUniform(cy::Matrix4f proj, cy::Matrix4f view, cy::Matrix4f lightproj, cy::Matrix4f lightview,cy::Point3f lightpos,int mode, int buffer_index) {
		mvp = proj * view * model * scale;
		cy::Matrix4f mvp_wo_proj = model * scale;

		glslProgram.SetUniform(0, mvp);
		glslProgram.SetUniform(1, mvp_wo_proj.GetSubMatrix3());
		if (mode != 1) {
			glslProgram.SetUniform(2, lightproj * lightview * model * scale);
		}
		else {
			glslProgram.SetUniform(2, mvp);
		}
		
		glslProgram.SetUniform(3, mode);
		glslProgram.SetUniform(4, lightpos);
		glslProgram.SetUniform(5, buffer_index);

	}

	void DrawArray() {
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(0);
	}

	int getvArraySize() {
		return 24;
	}

	cy::Point3f GetWorldPlaneNormal() {
		cy::Matrix4f m = model * scale;
		return cy::Point3f(m.GetTranspose().GetInverse() * pNormal).GetNormalized();
	}
	cy::Point3f GetWorldPlaneCenter() {
		cy::Matrix4f m = model * scale;
		return cy::Point3f(m * cy::Point4f(center, 1.0f));
	}

};

//-------------------------------------------------------------------------------

#endif
