#pragma once
#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_
#include "../glHeader.h"
#include "../loadpng/lodepng.h"
#include "Material.h"

class TriObj : public cyTriMesh
{

public:
	cy::GLSLProgram glslProgram;

    bool Load(const char *filename, bool loadMtl, bool useTexture)
    {
		this->USE_TEXTURE = useTexture;
        if ( ! LoadFromFileObj( filename, loadMtl ) ) return false;
        if ( ! HasNormals() ) ComputeNormals();
        ComputeBoundingBox();
		
		printf("%d faces found in object\n", NF());
		printf("%d vertices found in object\n", NV());
		printf("%d materials found in object\n", NM());

		printf("/////////////////////////////\n");
		printf("materials info:\n");
		for (int i = 0; i < NM(); i++) {
			printf("Material %d:\n", i);
			if(M(i).map_Ka)
			std::cout << "map_Ka: "<< M(i).map_Ka<<std::endl;
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

	void LoadTexture() {

		for (unsigned int i = 0; i < NM(); i++) {
			char* filename = M(i).map_Ka.data;
			if (LoadPNG(material[i], 0, filename)) material[i].hasKaTexture = true;

			filename = M(i).map_Kd.data;
			if(LoadPNG(material[i] ,1, filename)) material[i].hasKdTexture = true;

			filename = M(i).map_Ks.data;
			if(LoadPNG(material[i] ,2, filename)) material[i].hasKsTexture = true;

			/*
			filename = M(i).map_Ns.data;
			LoadPNG(material[i] ,3, filename);

			filename = M(i).map_d.data;
			LoadPNG(material[i] ,4, filename);

			filename = M(i).map_bump.data;
			LoadPNG(material[i] ,5, filename);

			filename = M(i).map_disp.data;
			LoadPNG(material[i],6, filename);
			*/
		}
		
	}

	bool LoadPNG(Material &Mtl,int index, const char* filename) {

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
	void initTexture() {
		for (unsigned int i = 0; i < NM(); ++i) {
			//std::cout << "i =  " << i << std::endl;
			Material *Mtl = &material[i];

			for (unsigned int index = 0; index < 7; index++) {
				if (!Mtl->textureMap[index].texture_data.size()) continue;
				Mtl->textureMap[index].gltexture.Initialize();
				Mtl->textureMap[index].gltexture.SetImageRGBA(Mtl->textureMap[index].texture_data.data(), Mtl->textureMap[index].width, Mtl->textureMap[index].height, 0);
				//diffuse_Texture.Bind();
				Mtl->textureMap[index].gltexture.BuildMipmaps();
			}
		}
	}

	void textureBind(int mid,int mapid,int bid) {
		material[mid].textureMap[mapid].gltexture.Bind(bid);
		/*for (unsigned int i = 0; i < NM(); i++) {
			diffuse_Texture.Bind();
		}*/
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
		return cy::Point3f(M(mid).Ka[0], M(mid).Ka[1], M(mid).Ka[2]);
	}
	
	cy::Point3f getMtlKd(int mid) {
		return cy::Point3f(M(mid).Kd[0], M(mid).Kd[1], M(mid).Kd[2]);
	}
	
	cy::Point3f getMtlKs(int mid) {
		return cy::Point3f(M(mid).Ks[0], M(mid).Ks[1], M(mid).Ks[2]);
	}
	
	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if(!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
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

    void computeArrayData(){
        vArrayPtr = new cy::Point3f[NF() * 3];
		nArrayPtr = new cy::Point3f[NF() * 3];
		tArrayPtr = new cy::Point3f[NF() * 3];
        unsigned int index = 0;
        for(unsigned int i=0; i<NF();i++){

            for(unsigned int j=0;j<3;j++){
               vArrayPtr[index] = V( F(i).v[j] );
			   //printf("point FN(i).v[j]: %d\n", FN(i).v[j]);
                nArrayPtr[index] = VN( FN(i).v[j] );
				tArrayPtr[index++] = VT(FT(i).v[j]);
            }
        }
		//printf("index: %d\n", index);
    }

    int getNumVertices(){
        return NV();
    }

	cy::Point3f* getVertices(){
		return &V(0);
        //return vertices;
    }

    int getvArraySize(){
        return NF()*3;
    }

    cy::Point3f* getvArrayPtr(){
        return vArrayPtr;
    }

    cy::Point3f* getnArrayPtr(){
        return nArrayPtr;
    }

	cy::Point3f* gettArrayPtr() {
		return tArrayPtr;
	}
protected:
	
	//cy::GLSLProgram vshader;
	//cy::GLSLProgram fshader;
	
private:
    //cyBVHTriMesh bvh;
	//std::vector<cy::Point3f> vArrayPtr, nArrayPtr;
    cy::Point3f *vArrayPtr, *nArrayPtr,*tArrayPtr;

	//texture data
	//cy::GLTexture2<GL_TEXTURE_2D> diffuse_Texture;
	//unsigned texture_width, texture_height;
	//std::vector<unsigned char> texture_data;

	Material *material;

	bool USE_TEXTURE;
};

//-------------------------------------------------------------------------------

#endif
