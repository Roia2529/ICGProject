#pragma once
#pragma once
#ifndef _PLANE_H_INCLUDED_
#define _PLANE_H_INCLUDED_
#include "BaseObject.h"

class PlaneObj : public BaseObject
{
private:
	cy::Matrix4f mvp;
	cy::Point4f pNormal = cy::Point4f(0.0f, 0.0f, 1.0f, 0.0f);

	float vertice[18] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

public:
	
	PlaneObj() : BaseObject() {
		//COLOR_MODE = mode;
	}

	virtual void setScale(GLfloat scaleinit) {
		scale.SetScale(scaleinit);

		scale.AddTrans(-center);

		model *= cy::Matrix4f::MatrixTrans(cy::Point3f(0, -2.0, 0));
		model *= cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);
		model *= cy::Matrix4f::MatrixRotationZ(-cy::cyPi<float>() / 2);
	}


	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");

		//for shading
		glslProgram.RegisterUniform(1, "framebuffer");

		//CY_GL_REGISTER_DEBUG_CALLBACK;
		return true;
	}

	void initBufferBind() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vbid = new GLuint[1];
		glGenBuffers(1, vbid);

		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*getvArraySize(), &vertice, GL_STATIC_DRAW);

	}

	void BufferBind() {
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void updateupdateUniform(cy::Matrix4f proj, cy::Matrix4f view, int buffer_index) {
		mvp = proj * view * model * scale;
		glslProgram.SetUniform(0, mvp);
	
		glslProgram.SetUniform(1, buffer_index);

		cy::Point4f sc = mvp * cy::Point4f(center, 1.0f);
		cy::Point2<int> scr_center = cy::Point2<int>(static_cast<int>(sc[0]/sc[3]*640), static_cast<int>(sc[1]/sc[3] * 480));
		glslProgram.SetUniform("center", scr_center);

	}

	void DrawArray() {
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
	}

	int getvArraySize() {
		return 18;
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
