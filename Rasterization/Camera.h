#ifndef CAM
#define CAM
#include<Eigen/Eigen>
class Camera {
public:
	Camera(const Camera& c);
	Camera(Eigen::Vector3f p, Eigen::Vector3f l, Eigen::Vector3f u,float f,float asp,float near,float far) 
		:position(p), lookAt(l), up(u) ,fov(f),aspect(asp),zNear(near),zFar(far){
		gxt = lookAt.cross(up);
	}
	Eigen::Vector3f get_position()const;
	Eigen::Vector3f get_lookAt()const;
	Eigen::Vector3f get_up()const;
	Eigen::Vector3f get_gxt()const;
	float get_fov() const { return fov; }
	float get_aspect() const { return aspect;}
	float get_zNear()const { return zNear; }
	float get_zFar()const { return zFar; }
	//set
	void set_position(Eigen::Vector3f);
	void set_lookAt(Eigen::Vector3f);
	void set_up(Eigen::Vector3f);
	void set_gxt();
	

private:
	Eigen::Vector3f position;
	Eigen::Vector3f lookAt;
	Eigen::Vector3f up;
	Eigen::Vector3f gxt;
	float fov;
	float aspect;
	float zNear;
	float zFar;
};
#endif