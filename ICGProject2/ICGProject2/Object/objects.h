#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_
#include "../glHeader.h"

class TriObj : public cyTriMesh
{

public:
    
    bool Load(const char *filename, bool loadMtl)
    {
        //bvh.Clear();
        if ( ! LoadFromFileObj( filename, loadMtl ) ) return false;
        if ( ! HasNormals() ) ComputeNormals();
        ComputeBoundingBox();
		computeVerticeArray();
        //bvh.SetMesh(this,4);
        return true;
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

    int getNumVertex(){
        return NF()*3;
    }

	cy::Point3f* getVertices(){
        return vertices;
    }
protected:
	int current_shader = -1;
	cy::GLSLProgram vshader;
	cy::GLSLProgram fshader;

private:
    //cyBVHTriMesh bvh;
    cy::Point3f *vertices;

};

//-------------------------------------------------------------------------------

#endif
