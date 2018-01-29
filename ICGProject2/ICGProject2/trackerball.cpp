#include "trackerball.h"

cy::Matrix4f trackball::getMatrix() {
	
	return cy::Matrix4f::MatrixTrans(cy::Point3f(0, 0, cur_dis - Default_distance)) * cur_matrix;
	//return cur_matrix;
}

void trackball::saveBeginRotate(cy::Point2f p) {
	pre_pos_2d = p;
	pre_pos = twodim2threedim(pre_pos_2d);
	pre_matrix = cur_matrix;
}

void trackball::calculateRotate(cy::Point2f p) {
	cur_pos_2d = p;
	cur_pos = twodim2threedim(cur_pos_2d);
	cy::Matrix4f rotateTheta;
	if (USE_TRACK) {
		float dot = pre_pos.Dot(cur_pos);
		cy::Point3f N = (pre_pos.Cross(cur_pos));
		N.Normalize(); //if it is not normalized, the matrix will contain zooming factor.
		float theta = dot / pre_pos.Length() / cur_pos.Length();

		//cy::Matrix4f rotateTheta;
		rotateTheta.SetRotation(N, theta);
		//cur_matrix = rotateTheta * pre_matrix;
	}
	else {
		//x
		float theta_x = +180 * (cur_pos_2d[0] - pre_pos_2d[0]) / 200.0f;
		//y
		float theta_y = -180 * (cur_pos_2d[1] - pre_pos_2d[1]) / 200.0f;
		rotateTheta = cy::Matrix4f::MatrixRotationX(theta_y) * cy::Matrix4f::MatrixRotationY(theta_x);
		
	}
	cur_matrix = rotateTheta * pre_matrix;
}

void trackball::saveBeginZoom(cy::Point2f p) {
	pre_dis_pos = p;
	//printf("save p[0]:%f, p[1]:%f\n", pre_dis_pos[0], pre_dis_pos[1]);
	pre_dis = cur_dis;
}

void trackball::calculateZoom(cy::Point2f p) {

	cur_dis = pre_dis + p[1] - pre_dis_pos[1];// +p[1] - pre_dis_pos[1];
	//printf("zoom dis:%f\n", cur_dis);
}

cy::Point3f trackball::twodim2threedim(cy::Point2f p) {
	float r = p[0] * p[0] + p[1] * p[1];
	float z = r > 1.0f / 2 ? 1.0f / 2 / sqrt(r) : sqrt(1.0f - r);
	return cy::Point3f(p[0], p[1], z);
}