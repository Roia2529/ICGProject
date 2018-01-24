#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_

#include "cyTriMesh.h"

class TriObj : public cyTriMesh
{
public:
    
    bool Load(const char *filename, bool loadMtl)
    {
        bvh.Clear();
        if ( ! LoadFromFileObj( filename, loadMtl ) ) return false;
        if ( ! HasNormals() ) ComputeNormals();
        ComputeBoundingBox();
        //bvh.SetMesh(this,4);
        return true;
    }

    void computeVerticeArray(){
        vertices = new GLfloat[NF()*3];
        unsigned int index = 0;
        for(unsigned int i=0; i<NF();i++){

            for(unsigned int j=0;j<3;j++){
                vertices[index++] = &V( F(i).v[j] );
            }
        }
    }

    int getNumVertex(){
        return NF()*3;
    }

    GLfloat getVertices(){
        return vertices;
    }
private:
    //cyBVHTriMesh bvh;
    Point3f *vertices;

};

//-------------------------------------------------------------------------------

#endif
