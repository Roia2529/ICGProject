#pragma once
#ifndef _OBJECTS_H_INCLUDED_
#define _OBJECTS_H_INCLUDED_
//#include "../glHeader.h"
//#include "../loadpng/lodepng.h"
//#include "Material.h"
#include "BaseObject.h"

class TriObj : public BaseObject
{
private:
	int COLOR_MODE;
	cy::Matrix4f mvp, mvp_wo_proj;
	cy::Matrix4f proj;
	cy::Matrix3f mvp_inv_trans;
	cy::Point3f lightpos;
	cy::Point3f camerapos;

public:
	TriObj(int mode) : BaseObject() {
		COLOR_MODE = mode;
	}

    bool Load(const char *filename, bool loadMtl, bool useTexture)
    {
		USE_TEXTURE = useTexture;
        if ( ! LoadFromFileObj( filename, loadMtl ) ) return false;
        if ( ! HasNormals() ) ComputeNormals();
        ComputeBoundingBox();
		
		printf("%d faces found in object\n", NF());
		printf("%d vertices found in object\n", NV());
		printf("%d materials found in object\n", NM());

		if (NM())
			printf("*-----materials info:-----*\n");
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
			printf("-----Success to load PNG-----\n");
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
	}

	void initBufferBind() {
		initTexture();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vbid = new GLuint[3];
		glGenBuffers(3, vbid);

		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*getvArraySize(), vArrayPtr, GL_STATIC_DRAW);

		//normals
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*getvArraySize(), getnArrayPtr(), GL_STATIC_DRAW);

		//texture
		if (USE_TEXTURE)
		{
			initTexture();
			glBindBuffer(GL_ARRAY_BUFFER, vbid[2]);

			glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Point3f)*getvArraySize(), gettArrayPtr(), GL_STATIC_DRAW);

		}
	}
	void BufferBind() {
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//Bind buffer before glVertexAttribPointer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbid[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


		if (USE_TEXTURE)
		{
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, vbid[2]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
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
		glslProgram.RegisterUniform(14, "cameraPos");
		glslProgram.RegisterUniform(15, "skybox");

		
		CY_GL_REGISTER_DEBUG_CALLBACK;
		return true;
	}

	void updateUniform(cy::Matrix4f new_mvp_wo_pro, cy::Matrix4f new_proj, cy::Matrix4f view, cy::Point3f newlightpos, cy::Point3f newcamerapos) {
		
		mvp_wo_proj = new_mvp_wo_pro;
		proj = new_proj;
		lightpos = newlightpos;

		mvp = proj * view * mvp_wo_proj;
		mvp_inv_trans = mvp_wo_proj.GetSubMatrix3().GetTranspose().GetInverse();

		camerapos = newcamerapos;
	}

	virtual void DrawArray() {

		int size = NM() > 0 ? NM() : 1;
		for (unsigned int i = 0; i < size; i++) {

			//glslProgram.SetUniform(0, mvp);
			glslProgram.SetUniform(0, mvp);
			glslProgram.SetUniform(1, mvp_inv_trans);
			glslProgram.SetUniform(2, mvp_wo_proj.GetSubMatrix3());
			glslProgram.SetUniform(3, lightpos);

			glslProgram.SetUniform(4, getMtlKa(i));
			glslProgram.SetUniform(5, getMtlKd(i));
			glslProgram.SetUniform(6, getMtlKs(i));

			glslProgram.SetUniform(7, COLOR_MODE);


			if (USE_TEXTURE)
			{
				glslProgram.SetUniform(8, 0);
				textureBind(i, 0, 0);
				glslProgram.SetUniform(9, 1);
				textureBind(i, 1, 1);
				glslProgram.SetUniform(10, 2);
				textureBind(i, 2, 2);

				glslProgram.SetUniform(11, hasTextureKa(i));
				glslProgram.SetUniform(12, hasTextureKd(i));
				glslProgram.SetUniform(13, hasTextureKs(i));
			}

			glslProgram.SetUniform(14, camerapos);
			glslProgram.SetUniform(15, 3);

			int offset = NM() > 0 ? GetMaterialFirstFace(i) * 3 : 0;
			int vsize = NM() > 0 ? GetMaterialFaceCount(i) * 3 : getvArraySize();
			glDrawArrays(GL_TRIANGLES, offset, vsize);
	
		}
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		if (USE_TEXTURE)
			glDisableVertexAttribArray(2);
	}

protected:
	
};

//-------------------------------------------------------------------------------

#endif
