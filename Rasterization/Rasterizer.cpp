#include"Rasterizer.h"
#include"Camera.h"
#include<limits>
#include<iostream>
#include<array>
#include"Shader.h"
using namespace rst;
static Eigen::Vector4f toVec4(Eigen::Vector3f v3,float w) {
	return Eigen::Vector4f(v3.x(), v3.y(), v3.z(), w);
}
Rasterizer::Rasterizer(Window w,Camera c) :window(w.width, w.height),
camera(c){
	frame_buf.resize(w.width * w.height);
	depth_buf.resize(w.width * w.height);
}
Eigen::Matrix4f Rasterizer::get_mvp() {
	return projection * view * model;
}

void Rasterizer::set_model_matrix(Eigen::Matrix4f m){
	model = m;
}
void Rasterizer::set_view_matrix(Eigen::Matrix4f v){
	view = v;
}
void Rasterizer::set_projection_matrix(Eigen::Matrix4f p){
	projection = p;
}
void Rasterizer::clear(Buffers buff) {
	if ((buff & Buffers::frame)==Buffers::frame) {
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0,0,0 });
	}
	if ((buff & Buffers::depth) == Buffers::depth) {
		std::fill(depth_buf.begin(),depth_buf.end(),std::numeric_limits<float>::infinity());
	}
	return ;
}
void rst::Rasterizer::draw(std::vector<Triangle*>& ts, int i){
	float f1 = (camera.get_zFar() - camera.get_zNear()) / 2;
	float f2 = (camera.get_zFar() + camera.get_zNear()) / 2;
	Eigen::Matrix4f mvp = this->get_mvp();
	for (const auto& t : ts) {
		Triangle newTri =*t;
		//mvp->viewport:坐标
		Eigen::Vector4f v[] = {
			mvp * t->v[0],
			mvp * t->v[1],
			mvp * t->v[2]
		};
		///齐次化
		for (auto& _v : v) {
			_v.x() /= _v.w();
			_v.y() /= _v.w();
			_v.z() /= _v.w();
		}
		///viewport transformation
		for (auto& _v : v) {
			_v.x() *= 0.5 * window.width;
			_v.x() += 0.5 * window.width;
			_v.y() *= 0.5 * window.width;
			_v.y() += 0.5 * window.width;
			_v.z() = _v.z() * f1 + f2;
		}
		//viewspace:坐标+法向量
		Eigen::Matrix4f mv = this->view * this->model;
		std::array<Eigen::Vector3f, 3> v_space = {
			(mv * t->v[0]).head<3>(),
			(mv * t->v[1]).head<3>(),
			(mv * t->v[2]).head<3>(),

		};
		///viewspace 坐标
		int i = 0;
		///viewspace 法向量
		Eigen::Matrix4f mv_it = mv.inverse().transpose();
		Eigen::Vector4f n_space[] = {
			mv_it * toVec4(t->normal[0],0.0f),
			mv_it * toVec4(t->normal[1],0.0f),
			mv_it * toVec4(t->normal[2],0.0f),
		};
		for (int i = 0 ; i < 3; ++i) {
			newTri.set_vertex(i, v[i]);
			newTri.set_color(i, Eigen::Vector3f(148.0f, 121.0f, 92.0f));
			newTri.set_normal(i, n_space[i].head<3>());
		}
		rasterize_triangle(newTri, v_space);
	}
}
void Rasterizer::draw(const std::vector<Triangle*>& ts) {
	float f1=(camera.get_zFar()-camera.get_zNear())/2;
	float f2 = (camera.get_zFar() + camera.get_zNear()) / 2;
	Eigen::Matrix4f mvp= this->get_mvp();
	for (const auto& t : ts) {
		Triangle newTri ;
		//mvp->viewport:坐标
		Eigen::Vector4f v[] = {
			mvp * t->v[0],
			mvp * t->v[1],
			mvp * t->v[2]
		};
		///齐次化
		for (auto& _v : v) {
			_v.x() /= _v.w();
			_v.y() /= _v.w();
			_v.z() /= _v.w();
		}
		///viewport transformation
		for (auto& _v : v) {
			_v.x() *= 0.5 * window.width;
			_v.x() += 0.5 * window.width;
			_v.y() *= 0.5 * window.width;
			_v.y() += 0.5 * window.width;
			_v.z() = _v.z() * f1 + f2;
		}
		for (int i = 0;i < 3;++i) {
			newTri.set_vertex(i, v[i]);
			newTri.set_color(i, t->color[i]);
		}
		rasterize_triangle(newTri);
		//viewspace:坐标+法向量
	}
}
bool insideTriangle(Triangle& t,float x,float y) {
	Eigen::Vector2f AB = (t.v[1] - t.v[0]).head<2>();
	Eigen::Vector2f BC = (t.v[2] - t.v[1]).head<2>();
	Eigen::Vector2f CA = (t.v[0] - t.v[2]).head<2>();
	Eigen::Vector2f P;
	P << x, y;
	Eigen::Vector2f AP = P - t.v[0].head<2>();
	Eigen::Vector2f BP = P - t.v[1].head<2>();
	Eigen::Vector2f CP = P - t.v[2].head<2>();
	bool isInside = false;
	if (AB.x() * AP.y() - AB.y() * AP.x() > 0 && BC.x() * BP.y() - BC.y() * BP.x() > 0 && CA.x() * CP.y() - CA.y() * CP.x() > 0) {
		isInside = true;
	}
	else if (AB.x() * AP.y() - AB.y() * AP.x() < 0 && BC.x() * BP.y() - BC.y() * BP.x() < 0 && CA.x() * CP.y() - CA.y() * CP.x() < 0) {
		isInside = true;
	}
	return isInside;
}
static std::tuple<float, float, float> Barycentric(float x, float y, const std::array<Eigen::Vector4f,3>& v) {
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}
void rst::Rasterizer::rasterize_triangle(Triangle& t){
	//bounding box
	float min_x = std::min(t.v[0].x(),std::min(t.v[1].x(),t.v[2].x()));
	float min_y = std::min(t.v[0].y(),std::min(t.v[1].y(),t.v[2].y()));
	float max_x = std::max(t.v[0].x(),std::max(t.v[1].x(),t.v[2].x()));
	float max_y = std::max(t.v[0].y(),std::max(t.v[1].y(),t.v[2].y()));
	min_x = std::floor(min_x);
	min_y = std::floor(min_y);
	max_x = std::ceil(max_x);
	max_y = std::ceil(max_y);
	//z-buffer
	for (int x = min_x;x <= max_x;++x) {
		for (int y = min_y;y <= max_y;++y) {
			if (insideTriangle(t, x, y)) {
				auto tmp = Barycentric(x, y, t.v);
				float alpha, beta, gamma;
				std::tie(alpha, beta, gamma) = tmp;
				//透视矫正
				float w_reciprocal = 1.0 / (alpha / t.v[0].w() + beta / t.v[1].w() + gamma / t.v[2].w());
				float z_interpolated = alpha * t.v[0].z() / t.v[0].w() + beta * t.v[1].z() / t.v[1].w() + gamma * t.v[2].z() / t.v[2].w();
				z_interpolated *= w_reciprocal;
				int id = get_index(x, y);
				if (z_interpolated < depth_buf[id]) {
					depth_buf[id] = z_interpolated;
					frame_buf[id] = t.color[0];
				}
			}
		}
	}
}
Eigen::Vector3f lerp_3d(float alpha, float beta, float gamma, const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const Eigen::Vector3f& v3, float weight) {
	return (alpha * v1 + beta * v2 + gamma * v3) / weight;
}
Eigen::Vector2f lerp_2d(float alpha, float beta, float gamma, const Eigen::Vector2f& v1, const Eigen::Vector2f& v2, const Eigen::Vector2f& v3, float weight){
	return (alpha * v1 + beta * v2 + gamma * v3) / weight;
}
void rst::Rasterizer::rasterize_triangle(Triangle& t, std::array<Eigen::Vector3f,3> pos){
	//bounding box
	float min_x = std::min(t.v[0].x(), std::min(t.v[1].x(), t.v[2].x()));
	float min_y = std::min(t.v[0].y(), std::min(t.v[1].y(), t.v[2].y()));
	float max_x = std::max(t.v[0].x(), std::max(t.v[1].x(), t.v[2].x()));
	float max_y = std::max(t.v[0].y(), std::max(t.v[1].y(), t.v[2].y()));
	min_x = std::floor(min_x);
	min_y = std::floor(min_y);
	max_x = std::ceil(max_x);
	max_y = std::ceil(max_y);
	for (int x = min_x;x <= max_x;++x) {
		for (int y = min_y;y <= max_y;++y) {
			if (insideTriangle(t, x, y)) {
				auto tmp = Barycentric(x+0.5, y+0.5, t.v);
				float alpha, beta, gamma;
				std::tie(alpha, beta, gamma) = tmp;
				//透视矫正
				float w_reciprocal = 1.0 / (alpha / t.v[0].w() + beta / t.v[1].w() + gamma / t.v[2].w());
				float z_interpolated = alpha * t.v[0].z() / t.v[0].w() + beta * t.v[1].z() / t.v[1].w() + gamma * t.v[2].z() / t.v[2].w();
				z_interpolated *= w_reciprocal;
				int id = get_index(x, y);
				if (z_interpolated < depth_buf[id]) {
					//[alpha, beta, gamma]是（x,y)的重心坐标，因此通过这个，去根据三维空间下的向量lerp出想要的值
					auto lerp_color = lerp_3d(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
					auto lerp_normal = lerp_3d(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1);
					//没有形变的坐标，用于计算shader光照
					auto lerp_shading_coords = lerp_3d(alpha, beta, gamma, pos[0], pos[1], pos[2], 1);
					auto lerp_texture_uv = lerp_2d(alpha, beta, gamma, t.texture_coord[0], t.texture_coord[1], t.texture_coord[2], 1);
					fragment_shader_payload payload(lerp_shading_coords, lerp_normal, lerp_color, camera.get_position(),lerp_texture_uv, texture ? &*texture : nullptr);
					Eigen::Vector3f shader_color = Eigen::Vector3f{ 0.0f,0.0f,0.0f }; //fragment_shader(payload);
					set_pixel(Eigen::Vector2i{ x,y }, shader_color);
				}
			}
		}
	}
}
void rst::Rasterizer::set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color)
{
	//old index: auto ind = point.y() + point.x() * width;
	int ind = (window.height - point.y()) * window.width + point.x();
	frame_buf[ind] = color;
}
