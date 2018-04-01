#pragma once
#ifndef _BASEOBJECT_H_INCLUDED_
#define _BASEOBJECT_H_INCLUDED_
#include "../glHeader.h"
#include "../loadpng/lodepng.h"
#include "Material.h"

class BaseObject : public cyTriMesh
{
protected:
	GLuint vao;
	GLuint *vbid;
	cy::Point3f *vArrayPtr, *nArrayPtr, *tArrayPtr;
	cy::GLSLProgram glslProgram;
	Material *material;

	cy::Point3f center;
	cy::Matrix4f model;
	cy::Matrix4f scale;

	bool USE_TEXTURE;

public:
	BaseObject()
	{
		center = cy::Point3f(0, 0, 0);
		USE_TEXTURE = false;
		model.SetIdentity();
	}

	virtual void setScale(GLfloat scaleinit) {
		scale.SetScale(scaleinit);

		printf("BOUND MIN: %f %f %f\n", GetBoundMin().x * scaleinit, GetBoundMin().y * scaleinit, GetBoundMin().z * scaleinit);
		printf("BOUND MAX: %f %f %f\n", GetBoundMax().x * scaleinit, GetBoundMax().y * scaleinit, GetBoundMax().z * scaleinit);
		//move object to center
		center = (GetBoundMin() + GetBoundMax())*scaleinit*(0.5);

		//set center
		//center = cy::Point3f(15, 15, 15);
		scale.AddTrans(-center);
	}

	cy::Matrix4f getScale() {
		return scale;
	}

	void programBind() {
		glslProgram.Bind();
	}

	void computeArrayData() {
		vArrayPtr = new cy::Point3f[NF() * 3];
		if (HasNormals()) nArrayPtr = new cy::Point3f[NF() * 3];
		if (HasTextureVertices()) tArrayPtr = new cy::Point3f[NF() * 3];
		unsigned int index = 0;
		for (unsigned int i = 0; i<NF(); i++) {

			for (unsigned int j = 0; j<3; j++) {
				vArrayPtr[index] = V(F(i).v[j]);
				if (HasNormals()) {
					nArrayPtr[index] = VN(FN(i).v[j]);
				}
				if(HasTextureVertices())tArrayPtr[index] = VT(FT(i).v[j]);
				index++;
			}
		}
		std::cout << "getvArraySize: " << index << std::endl;
	}

	bool hasTextureKa(int mid) {
		return material[mid].hasKaTexture;
	}

	bool hasTextureKd(int mid) {
		return material[mid].hasKdTexture;
	}
	bool hasTextureKs(int mid) {
		return material[mid].hasKsTexture;
	}

	cy::Point3f getMtlKa(int mid) {
		if(NM())
			return cy::Point3f(M(mid).Ka[0], M(mid).Ka[1], M(mid).Ka[2]);
		return cy::Point3f(0.2, 0.2, 0.2);
	}

	cy::Point3f getMtlKd(int mid) {
		if (NM())
			return cy::Point3f(M(mid).Kd[0], M(mid).Kd[1], M(mid).Kd[2]);
		return cy::Point3f(0.2, 0.2, 0.2);
	}

	cy::Point3f getMtlKs(int mid) {
		if (NM())
			return cy::Point3f(M(mid).Ks[0], M(mid).Ks[1], M(mid).Ks[2]);
		return cy::Point3f(1.0, 1.0, 1.0);
	}
	int getNumVertices() {
		return NV();
	}

	cy::Point3f* getVertices() {
		return &V(0);
		//return vertices;
	}

	virtual int getvArraySize() {
		return NF() * 3;
	}

	cy::Point3f* getvArrayPtr() {
		return vArrayPtr;
	}

	cy::Point3f* getnArrayPtr() {
		return nArrayPtr;
	}

	cy::Point3f* gettArrayPtr() {
		return tArrayPtr;
	}

	virtual bool Load(const char *filename, bool loadMtl, bool useTexture)
	{
		this->USE_TEXTURE = useTexture;
		if (!LoadFromFileObj(filename, loadMtl)) return false;
		if (!HasNormals()) ComputeNormals();
		ComputeBoundingBox();

		printf("%d faces found in object\n", NF());
		printf("%d vertices found in object\n", NV());
		printf("%d materials found in object\n", NM());

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

	virtual void LoadTexture() {

		for (unsigned int i = 0; i < NM(); i++) {
			char* filename = M(i).map_Ka.data;
			if (LoadPNG(material[i], 0, filename)) material[i].hasKaTexture = true;

			filename = M(i).map_Kd.data;
			if (LoadPNG(material[i], 1, filename)) material[i].hasKdTexture = true;

			filename = M(i).map_Ks.data;
			if (LoadPNG(material[i], 2, filename)) material[i].hasKsTexture = true;
			printf("-----Success to load PNG-----\n");
		}


	}

	virtual bool LoadPNG(Material &Mtl, int index, const char* filename) {

		if (filename == NULL) return FALSE;
		unsigned error = lodepng::decode(Mtl.textureMap[index].texture_data, Mtl.textureMap[index].width, Mtl.textureMap[index].height, filename);

		// If there's an error, display it.
		if (error != 0)
		{
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			//return ;
		}
		std::cout << "Load " << filename << " done." << std::endl;
		return true;
	}

	//call this after gl init
	virtual void initTexture() {}

	virtual void textureBind(int mid, int mapid, int bid) {
		material[mid].textureMap[mapid].gltexture.Bind(bid);
	}

	virtual bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) { return true; }

	virtual void initBufferBind() {}
	virtual void BufferBind() {}
	virtual void DrawArray() {}

protected:

	//cy::GLSLProgram vshader;
	//cy::GLSLProgram fshader;

};

//-------------------------------------------------------------------------------

#endif
#pragma once
