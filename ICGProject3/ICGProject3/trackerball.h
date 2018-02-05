#pragma once
#include <math.h>
#include "glHeader.h"

class trackball {
public:
	/** constractors */
	trackball()
	{
		//I
		cur_matrix.SetIdentity();
		matrix.SetIdentity();
		USE_TRACK = false;
	}

	/*
	* convert mouse position to window position x:[-1,1] y:[-1,1]
	*/
	inline void mpos2window(int x, int y, float width, float height, cy::Point2f& p)
	{
		p = cy::Point2f(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
	}
	cy::Matrix4f getMatrix();

	void saveBeginRotate(cy::Point2f p);
	void calculateRotate(cy::Point2f p);
	void saveBeginZoom(cy::Point2f p);
	void calculateZoom(cy::Point2f p);
private:
	float angle = 0.0, axis[3], trans[3];

	cy::Matrix4f cur_matrix;
	cy::Matrix4f pre_matrix;
	cy::Point3f  cur_pos;
	cy::Point3f  pre_pos;
	cy::Point2f  cur_pos_2d;
	cy::Point2f  pre_pos_2d;

	cy::Matrix4f matrix;

	cy::Point3f twodim2threedim(cy::Point2f p);

	bool USE_TRACK;
	float Default_distance = 2.0f;
	float cur_dis = 0;
	float pre_dis = 0;
	cy::Point2f pre_dis_pos;

};

extern trackball tkball;
extern trackball light;