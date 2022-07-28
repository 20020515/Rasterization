#include"Camera.h"

Camera::Camera(const Camera& c){
	position=c.position;
	lookAt=c.lookAt;
	up=c.up;
	gxt=c.gxt;
	fov=c.fov;
	aspect=c.aspect;
	zNear=c.zNear;
	zFar=c.zFar;
}

Eigen::Vector3f Camera::get_position()const{
	return position;
}

Eigen::Vector3f Camera::get_lookAt()const{
	return lookAt;
}

Eigen::Vector3f Camera::get_up()const{
	return up;
}

Eigen::Vector3f Camera::get_gxt() const{
	return gxt;
}

void Camera::set_position(Eigen::Vector3f p){
	position = p;
}

void Camera::set_lookAt(Eigen::Vector3f l){
	lookAt = l;
}

void Camera::set_up(Eigen::Vector3f u){
	up = u;
}

void Camera::set_gxt(){
	gxt = lookAt.cross(up);
}
