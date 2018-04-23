#pragma once
#ifndef _CUBEMAP_H_INCLUDED_
#define _CUBEMAP_H_INCLUDED_
#include "../glHeader.h"
#include "../loadpng/lodepng.h"

class CubeMap : public cyTriMesh {

private:
	GLuint vao;
	GLuint *vbid;
	//unsigned int textureID;

	std::vector<std::string> figure_names;
	CubeFace textureMap[6];
	cy::GLTextureCubeMap tcubemap;
	cy::Point3f *vArrayPtr, *nArrayPtr, *tArrayPtr;
	cy::Point3f center;

	cy::Matrix4f scale;
	cy::Matrix4f projection;
	bool USE_TEXTURE;
	cy::GLSLProgram glslProgram;

public:					//! Constructor sets the default material values
	CubeMap()
	{
		this->USE_TEXTURE = false;
	}
	

	void setScale(GLfloat scaleinit) {
		scale.SetScale(scaleinit);

		//center
		center = (GetBoundMin() + GetBoundMax())*scaleinit*(0.5);
		
		scale.AddTrans(-center);

		//place transition
		scale.AddTrans(cy::Point3f(0,8.8,0));
	}

	cy::Matrix4f getScale() {
		return scale;
	}
	bool Load(const char *filename, bool loadMtl, bool useTexture, std::vector<std::string> figurenames)
	{
		this->USE_TEXTURE = useTexture;
		this->figure_names= figurenames;

		if (!LoadFromFileObj(filename, loadMtl)) return false;
		if (!HasNormals()) {
			ComputeNormals();
		}
		ComputeBoundingBox();
		setScale(1);
		computeArrayData();
		if(useTexture)LoadTexture();
		return true;
	}

	bool LoadTexture() {
		std::cout << "Start Loading Sky box figures" << std::endl;
		for (unsigned int i = 0; i < 6; i++) {
			const char* filename = this->figure_names[i].c_str();
			if (!LoadPNG(i, filename)) {
				std::cout << "Load cube figure failed." << std::endl;
				return false;
			}
		}
		printf("-----Success to load PNG-----\n");
		return true;
	}

	void initTexture() {
		tcubemap.Initialize();
		for (unsigned int i = 0; i < 6; i++) {
			//tcubemap.SetImageRGBA((cy::GLTextureCubeMap::Side)i, textureMap[i].texture_data.data(), textureMap[i].width, textureMap[i].height, 0);
			tcubemap.SetImage((cy::GLTextureCubeMap::Side)i, GL_RGBA4, GL_RGBA, (GLubyte*)textureMap[i].texture_data.data(), textureMap[i].width, textureMap[i].height,0);
			CY_GL_REGISTER_DEBUG_CALLBACK;
		}
		tcubemap.BuildMipmaps();
		tcubemap.SetSeamless(true);
		tcubemap.SetMaxAnisotropy();
		
		tcubemap.SetFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
		//CY_GL_REGISTER_DEBUG_CALLBACK;
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

	void initBufferBind() {
		if(this->USE_TEXTURE)initTexture();

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

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if (!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");
		if (this->USE_TEXTURE)glslProgram.RegisterUniform(1, "skybox");

		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

	void updateUniform(cy::Matrix4f project, cy::Matrix4f view) {
		cy::Matrix4f mat = project * view * scale;
		glslProgram.SetUniform(0, mat);
		glslProgram.SetUniform("matInvTrans", scale.GetSubMatrix3().GetTranspose().GetInverse());
		glslProgram.SetUniform("matOrigin", scale.GetSubMatrix3());

		if (this->USE_TEXTURE) {
			tcubemap.Bind(1);
			glslProgram.SetUniform(1, 1);
		}
	}

	void textureBind(int textureid) {
		tcubemap.Bind(textureid);
	}

	void BufferBind() {
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void DrawArray() {
		glDrawArrays(GL_TRIANGLES, 0, 3 * NF());
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	void computeArrayData() {
		//printf("NF: %d", NF());
		vArrayPtr = new cy::Point3f[NF() * 3];
		if(HasNormals())nArrayPtr = new cy::Point3f[NF() * 3];
		unsigned int index = 0;
		for (unsigned int i = 0; i<NF(); i++) {

			for (unsigned int j = 0; j<3; j++) {
				vArrayPtr[index] = V(F(i).v[j]);
				if (HasNormals()) {
					nArrayPtr[index] = VN(FN(i).v[j]);
				}
				index++;
			}
		}
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