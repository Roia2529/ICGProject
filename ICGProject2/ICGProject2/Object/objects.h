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
		computeVerticeArray();
		printf("%d faces found in object\n", NF());
		printf("%d vertices found in object\n", NV());
        //bvh.SetMesh(this,4);
        return true;
    }

	bool initGLSLProgram(const char *vShaderFile, const char *fShaderFile) {
		if(!glslProgram.BuildFiles(vShaderFile, fShaderFile)) return false;
		glslProgram.Bind();
		glslProgram.RegisterUniform(0, "mat");
		return true;
	}

	void programBind() {
		glslProgram.Bind();
	}

    void computeVerticeArray(){
        vertices = new cy::Point3f[NF()*3];
        unsigned int index = 0;
        for(unsigned int i=0; i<NF();i++){

            for(unsigned int j=0;j<3;j++){
                vertices[index++] = V( F(i).v[j] );
            }
        }
    }

    int getNumVertices(){
        return NV();
    }

	cy::Point3f* getVertices(){
		return &V(0);
        //return vertices;
    }
protected:
	
	//cy::GLSLProgram vshader;
	//cy::GLSLProgram fshader;
	
private:
    //cyBVHTriMesh bvh;
    cy::Point3f *vertices;

};

//-------------------------------------------------------------------------------

#endif
