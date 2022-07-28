#include<opencv2/opencv.hpp>
#include<Eigen/Eigen>
#include"Rasterizer.h"
#include"Triangle.h"
#include"Camera.h"
#include"global.h"
#include"Window.h"
#include"OBJ_Loader.h"
#include<functional>
#include"Shader.h"
#include<iostream>
Eigen::Matrix4f get_model_matrix(float angle) {
	double r = angle * PI / 180.0f;
	Eigen::Matrix4f m;
	m << cos(r), 0, sin(r), 0,
		0, 1, 0, 0,
		-sin(r), 0, cos(r), 0,
		0, 0, 0, 1;
	Eigen::Matrix4f scale;
	scale << 2.5, 0, 0, 0,
		0, 2.5, 0, 0,
		0, 0, 2.5, 0,
		0, 0, 0, 1;
	return m*scale;
}
Eigen::Matrix4f get_view_matrix(const Camera& c) {
	Eigen::Matrix4f v,t;
	t << 1, 0, 0, c.get_position().x(),
		0, 1, 0, c.get_position().y(),
		0, 0, 1, c.get_position().z(),
		0, 0, 0, 1;
	v << c.get_gxt().x(), c.get_up().x(), -c.get_lookAt().x(), 0,
		c.get_gxt().y(), c.get_up().y(), -c.get_lookAt().y(), 0,
		c.get_gxt().z(), c.get_up().z(), -c.get_lookAt().z(), 0,
		0, 0, 0, 1;
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
	view *= v.inverse();
	return view*t.inverse();
}
Eigen::Matrix4f get_projection_matrix(const Camera& c) {
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f perspective;
	float zNear = c.get_zNear();
	float zFar = c.get_zFar();
	perspective<<
		zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;
	double halfEyeRadian = c.get_fov() * PI / 2 / 180.0;
	double top = zNear * tan(halfEyeRadian);
	double bottom = -top;
	double right = top * c.get_aspect();
	double left = -right;
	Eigen::Matrix4f transfer;
	transfer <<
		1, 0, 0, -(right + left) / 2,
		0, 1, 0, -(top + bottom) / 2,
		0, 0, 1, -(zNear + zFar) / 2,
		0, 0, 0, 1;

	Eigen::Matrix4f s;
	s <<
		2 / (right - left), 0, 0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 2 / (zNear - zFar), 0,
		0, 0, 0, 1;

	return s * transfer * perspective;
}

int main(void) {
	Window window(700,700);
	
	Eigen::Vector3f position = { 0,0,10 };
	Eigen::Vector3f lookAt = { 0,0,1 };
	Eigen::Vector3f up = {0,1,0};
	Camera camera(position,lookAt,up,45,1,0.1,50);
	rst::Rasterizer r(window,camera);
	std::vector<Triangle*> triangleList;
	bool atriangle = false;
	if (atriangle) {
		std::vector<Eigen::Vector4f> pos
		{
				{2, 0, -2, 1},
				{0, 2, -2, 1},
				{-2, 0, -2, 1},
				{3.5, -1, -5, 1},
				{2.5, 1.5, -5, 1},
				{-1, 0.5, -5, 1},
		};

		std::vector<Eigen::Vector3f> cols
		{
				{217.0, 238.0, 185.0},
				{217.0, 238.0, 185.0},
				{217.0, 238.0, 185.0},
				{185.0, 217.0, 238.0},
				{185.0, 217.0, 238.0},
				{185.0, 217.0, 238.0},
		};

		for (int i = 0;i < pos.size();++i) {
			Triangle* tmp=new Triangle();
			tmp->set_vertex(0, pos[i]);
			tmp->set_color(0, cols[i++]);
			tmp->set_vertex(1, pos[i]);
			tmp->set_color(1, cols[i++]);
			tmp->set_vertex(2, pos[i]);
			tmp->set_color(2, cols[i]);
			triangleList.push_back(tmp);
		}
	}
	else {
		objl::Loader loader;
		std::string obj_path = "./models/spot";
		bool hasload = loader.LoadFile("./models/spot/spot_triangulated_good.obj");
		for (auto mesh : loader.LoadedMeshes) {
			for (int i = 0;i < mesh.Vertices.size();i += 3) {
				Triangle* tmp = new Triangle();
				for (int j = 0;j < 3;j++)
				{
					tmp->set_vertex(j, Eigen::Vector4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.0));
					tmp->set_normal(j, Eigen::Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z));
					tmp->set_texcoord(j, Eigen::Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
				}
				triangleList.push_back(tmp);
			}
		}
		//shader:
		std::function<Eigen::Vector3f(vertex_shader_payload)> vertex_shader = Shader::vertex_shader;
		std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader = Shader::phong_fragment_shader;
		r.set_fragment_shader(fragment_shader);
		r.set_vertext_shader(vertex_shader);
		r.set_texture(Texture("./models/spot/spot_texture.png"));
		int key = 0;
		float angle = 140;
		while (key != 27) {
			r.clear(rst::Buffers::depth | rst::Buffers::frame);
			r.set_model_matrix(get_model_matrix(angle));
			r.set_view_matrix(get_view_matrix(camera));
			r.set_projection_matrix(get_projection_matrix(camera));
			r.draw(triangleList,key);
			cv::Mat image(window.width, window.height, CV_32FC3, r.frame_buf.data());
			image.convertTo(image, CV_8UC3, 1.0f);
			cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
			cv::imshow("image", image);
			key = cv::waitKey(10);

		}
	}
	
}