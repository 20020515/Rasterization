#ifndef TRI
#define TRI
#include<Eigen/Eigen>
#include<vector>
#include<array>
class Triangle {
public:
	std::array<Eigen::Vector4f,3> v;
	std::array<Eigen::Vector3f,3> color;
	std::array<Eigen::Vector3f,3> normal;
	//(x,y)->(u,v)
	std::array<Eigen::Vector2f,3> texture_coord;
	void set_vertex(int ind, Eigen::Vector4f vec) {
		v[ind] = vec;
	}
	void set_color(int ind, Eigen::Vector3f col) {
		color[ind] = col;
	}
	void set_normal(int ind, Eigen::Vector3f norm) {
		normal[ind] = norm;
	}
	void set_texcoord(int ind, Eigen::Vector2f tex_coord) {
		texture_coord[ind] = tex_coord;
	}
};
#endif 
