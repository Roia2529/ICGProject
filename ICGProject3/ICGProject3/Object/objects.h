#pragma once
#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_
#include "../glHeader.h"

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
		computeArrayData();
		printf("%d faces found in object\n", NF());
		printf("%d vertices found in object\n", NV());
        //bvh.SetMesh(this,4);
        return true;
    }

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if(!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");

        //for shading
        glslProgram.RegisterUniform(1, "matInvTrans");
		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

    void computeArrayData(){
        vArrayPtr = new cy::Point3f[NF()*3];
		nArrayPtr = new cy::Point3f[NF() * 3];
        unsigned int index = 0;
        for(unsigned int i=0; i<NF();i++){

            for(unsigned int j=0;j<3;j++){
               vArrayPtr[index] = V( F(i).v[j] );
			   //printf("point FN(i).v[j]: %d\n", FN(i).v[j]);
                nArrayPtr[index++] = VN( FN(i).v[j] );
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
protected:
	
	//cy::GLSLProgram vshader;
	//cy::GLSLProgram fshader;
	
private:
    //cyBVHTriMesh bvh;
	//std::vector<cy::Point3f> vArrayPtr, nArrayPtr;
    cy::Point3f *vArrayPtr, *nArrayPtr;

};

//-------------------------------------------------------------------------------

#endif
