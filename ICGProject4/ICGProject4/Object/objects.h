#pragma once
#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_
#include "../glHeader.h"
#include "../loadpng/lodepng.h"

class TriObj : public cyTriMesh
{

public:
	cy::GLSLProgram glslProgram;

    bool Load(const char *filename, bool loadMtl)
    {
        //bvh.Clear();
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
			std::cout << "map_Ka: "<< M(i).map_Ka<<std::endl;
			std::cout << "map_Kd: " << M(i).map_Kd << std::endl;
			std::cout << "map_Ks: " << M(i).map_Ks << std::endl;
			printf("-------------------------\n");
		}

		computeArrayData();
		if (NM()) {
			LoadTexture();
		}
        return true;
    }

	void LoadTexture() {
		//assume one material
		char* filename = M(0).map_Kd.data;
		//printf("material path: %s\n", filename);
		unsigned error = lodepng::decode(texture_data, texture_width, texture_height, filename);

		// If there's an error, display it.
		if (error != 0)
		{
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			return ;
		}
		//initTexture();
	}

	//call this after gl init
	void initTexture() {
		if (!texture_data.size()) return;
		diffuse_Texture.Initialize();
		diffuse_Texture.SetImageRGBA(texture_data.data(), texture_width, texture_height, 0);
		//diffuse_Texture.Bind();
		diffuse_Texture.BuildMipmaps();
	}

	void textureBind() {
		diffuse_Texture.Bind();
	}
	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if(!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");

        //for shading
        glslProgram.RegisterUniform(1, "matInvTrans");
        glslProgram.RegisterUniform(2, "matOrigin");
        glslProgram.RegisterUniform(3, "lightP");
		glslProgram.RegisterUniform(4, "ourTexture");
		
        glslProgram.RegisterUniform(5, "colormode");
		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

    void computeArrayData(){
        vArrayPtr = new cy::Point3f[NF()*3];
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
		printf("index: %d\n", index);
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
	cy::GLTexture2<GL_TEXTURE_2D> diffuse_Texture;
	unsigned texture_width, texture_height;
	std::vector<unsigned char> texture_data;
};

//-------------------------------------------------------------------------------

#endif
