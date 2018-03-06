#pragma once
#ifndef _LIGHT_H_INCLUDED_
#define _LIGHT_H_INCLUDED_
#include "BaseObject.h"

class lightObj : public BaseObject
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

	cy::Point3f center;
	cy::GLSLProgram lightshader;
	GLuint vao_p, *vbid_p;
public:

	lightObj(cy::Point3f lpos) : BaseObject() {
		//COLOR_MODE = mode;
		model.AddTrans(lpos);
	}

	bool Load(const char *filename, bool loadMtl, bool useTexture)
	{
		USE_TEXTURE = useTexture;
		if (!LoadFromFileObj(filename, loadMtl)) return false;
		if (!HasNormals()) ComputeNormals();
		ComputeBoundingBox();

		printf("%d faces found in light\n", NF());
		printf("%d vertices found in light\n", NV());
		printf("%d materials found in light\n", NM());

		if (NM())
			printf("*-----materials info:-----*\n");
		for (int i = 0; i < NM(); i++) {
			printf("Material %d:\n", i);
			if (M(i).map_Ka)
				std::cout << "map_Ka: " << M(i).map_Ka << std::endl;
			if (M(i).map_Kd)
				std::cout << "map_Kd: " << M(i).map_Kd << std::endl;
			if (M(i).map_Ks)
				std::cout << "map_Ks: " << M(i).map_Ks << std::endl;
			printf("-------------------------\n");
		}

		computeArrayData();
		if (NM()) {
			material = new Material[NM()];
			LoadTexture();
		}
		return true;
	}

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		//glslProgram.RegisterUniform(0, "mat");
		//glslProgram.RegisterUniform(1, "matOrigin");
		//glslProgram.RegisterUniform(2, "shadowmvp");
		//glslProgram.RegisterUniform(3, "drawmode");
		//glslProgram.RegisterUniform(4, "lightP");
		//glslProgram.RegisterUniform(5, "shadowmap");

		//CY_GL_REGISTER_DEBUG_CALLBACK;
		//if (!lightshader.BuildFiles("vshader_light", "fshader_light"));

		return true;
	}

	void initBufferBind() {
		if (!lightshader.BuildFiles("vshader_light.glsl", "fshader_light.glsl")) return ;
		lightshader.Bind();
		lightshader.RegisterUniform(0, "mvp");

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vbid = new GLuint[2];
		glGenBuffers(2, vbid);

		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*getvArraySize(), vArrayPtr, GL_STATIC_DRAW);
		
		//normals
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*getvArraySize(), getnArrayPtr(), GL_STATIC_DRAW);


	}

	void initBufferBindPlane() {
		glslProgram.Bind();
		glGenVertexArrays(1, &vao_p);
		glBindVertexArray(vao_p);

		vbid_p = new GLuint[1];
		glGenBuffers(1, vbid_p);

		glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*18, &vertice, GL_STATIC_DRAW);

	}

	void BufferBind() {
		lightshader.Bind();
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	}

	void BufferBindPlane() {
		glBindVertexArray(vao_p);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid_p[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void updateFrameBuffer(int index) {
		glslProgram.SetUniform("tex", index);
	}

	void updateUniform(cy::Matrix4f mvp_wo_model) {
		mvp = mvp_wo_model * model * scale;
		//lightshader.SetUniform(0, mvp);
	}
	void DrawArray() {
		int size = NM() > 0 ? NM() : 1;
		for (unsigned int i = 0; i < size; i++) {

			//glslProgram.SetUniform(0, mvp);
			lightshader.SetUniform(0, mvp);

			int offset = NM() > 0 ? GetMaterialFirstFace(i) * 3 : 0;
			int vsize = NM() > 0 ? GetMaterialFaceCount(i) * 3 : getvArraySize();
			glDrawArrays(GL_TRIANGLES, offset, vsize);

		}
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void DrawArrayPlane() {
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDisableVertexAttribArray(0);
	}

	int getvArraySize() {
		return NF() * 3;
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
