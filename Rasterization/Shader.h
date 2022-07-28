#ifndef SHADER
#define SHADER
#include<Eigen/Eigen>
#include"Texture.h"
struct fragment_shader_payload
{
	Eigen::Vector3f shader_coords;
	Eigen::Vector3f normarl;
	Eigen::Vector3f color;
	Eigen::Vector3f eye_pos;
	Eigen::Vector2f uv;
	Texture* texture;
	fragment_shader_payload(const Eigen::Vector3f& sc, const Eigen::Vector3f& n, const Eigen::Vector3f eye,const Eigen::Vector3f& col,
		const Eigen::Vector2f& u, Texture* tex) :shader_coords(sc), normarl(n), color(col), eye_pos(eye),uv(u),texture(tex) {
		texture=nullptr;
	}
};
struct vertex_shader_payload
{

};
struct light {
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
	light(Eigen::Vector3f pos, Eigen::Vector3f i):position(pos),intensity(i) {
		;
	}
};
class Shader {
public:
	static Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload);
	static Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload);
	static Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload);
};
#endif // !SHADER
