#pragma once
#ifndef _CUBEMAP_H_INCLUDED_
#define _CUBEMAP_H_INCLUDED_
#include "../glHeader.h"
#include "../loadpng/lodepng.h"

class CubeMap : public cyTriMesh {

private:
	cy::GLSLProgram glslProgram;
	Texture textureMap[6];
	cy::Point3f *vArrayPtr, *nArrayPtr, *tArrayPtr;
	//Material *material;
	std::vector<std::string> figure_names;
	bool USE_TEXTURE;

public:					//! Constructor sets the default material values
	CubeMap()
	{
		this->USE_TEXTURE = false;
	}

	bool Load(const char *filename, bool loadMtl, bool useTexture, std::vector<std::string> figurenames)
	{
		this->USE_TEXTURE = useTexture;
		this->figure_names= figurenames;

		if (!LoadFromFileObj(filename, loadMtl)) return false;
		if (!HasNormals()) ComputeNormals();
		ComputeBoundingBox();

		computeArrayData();
		LoadTexture();
		return true;
	}

	void LoadTexture() {

		for (unsigned int i = 0; i < 6; i++) {
			const char* filename = this->figure_names[i].c_str();
			if (!LoadPNG(i, filename)) {
				std::cout << ":Load cube figure failed." << std::endl;
				break;
			}
		}
	}

	bool LoadPNG(int index, const char* filename) {

		if (filename == NULL) return FALSE;
		unsigned error = lodepng::decode(textureMap[index].texture_data, textureMap[index].width, textureMap[index].height, filename);

		// If there's an error, display it.
		if (error != 0)
		{
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			//return ;
		}
		std::cout << "Load " << filename << " done." << std::endl;
		return true;
	}

	void textureBind(int mid, int mapid, int bid) {

	}

	cy::Point3f getMtlKa(int mid) {
		return cy::Point3f(M(mid).Ka[0], M(mid).Ka[1], M(mid).Ka[2]);
	}

	cy::Point3f getMtlKd(int mid) {
		return cy::Point3f(M(mid).Kd[0], M(mid).Kd[1], M(mid).Kd[2]);
	}

	cy::Point3f getMtlKs(int mid) {
		return cy::Point3f(M(mid).Ks[0], M(mid).Ks[1], M(mid).Ks[2]);
	}

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");

		//for shading
		glslProgram.RegisterUniform(1, "matInvTrans");
		glslProgram.RegisterUniform(2, "matOrigin");
		glslProgram.RegisterUniform(3, "lightP");

		glslProgram.RegisterUniform(4, "Mtl_ka");
		glslProgram.RegisterUniform(5, "Mtl_kd");
		glslProgram.RegisterUniform(6, "Mtl_ks");

		glslProgram.RegisterUniform(7, "colormode");


		if (USE_TEXTURE)
		{
			glslProgram.RegisterUniform(8, "Tmap_ka");
			glslProgram.RegisterUniform(9, "Tmap_kd");
			glslProgram.RegisterUniform(10, "Tmap_ks");

			glslProgram.RegisterUniform(11, "Use_Tmap_ka");
			glslProgram.RegisterUniform(12, "Use_Tmap_kd");
			glslProgram.RegisterUniform(13, "Use_Tmap_ks");
		}


		CY_GL_REGISTER_DEBUG_CALLBACK;
		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

	void computeArrayData() {
		vArrayPtr = new cy::Point3f[NF() * 3];
		nArrayPtr = new cy::Point3f[NF() * 3];
		unsigned int index = 0;
		for (unsigned int i = 0; i<NF(); i++) {

			for (unsigned int j = 0; j<3; j++) {
				vArrayPtr[index] = V(F(i).v[j]);
				nArrayPtr[index] = VN(FN(i).v[j]);
			}
		}
	}

	int getNumVertices() {
		return NV();
	}

	cy::Point3f* getVertices() {
		return &V(0);
	}

	int getvArraySize() {
		return NF() * 3;
	}

	cy::Point3f* getvArrayPtr() {
		return vArrayPtr;
	}

	cy::Point3f* getnArrayPtr() {
		return nArrayPtr;
	}

	


};

#endif