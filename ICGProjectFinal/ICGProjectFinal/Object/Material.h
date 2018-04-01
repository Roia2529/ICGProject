#pragma once
#ifndef _MATERIAL_H_INCLUDED_
#define _MATERIAL_H_INCLUDED_
#include "../glHeader.h"

class Material {
	
private:
	//char*   name;		//!< Material name
	//char*   map_Ka;	//!< Ambient color texture map
	//char*   map_Kd;	//!< Diffuse color texture map
	//char*   map_Ks;	//!< Specular color texture map
	//char*   map_Ns;	//!< Specular exponent texture map
	//char*   map_d;	//!< Alpha texture map
	//char*   map_bump;	//!< Bump texture map
	//char*   map_disp;	//!< Displacement texture map
	
public:					//! Constructor sets the default material values
	Material()
	{
		hasKdTexture = false;
		hasKaTexture = false;
		hasKsTexture = false;
	}
	Texture textureMap[7];

	bool hasKdTexture;
	bool hasKaTexture;
	bool hasKsTexture;
};

#endif