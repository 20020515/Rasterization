#ifndef RAST
#define RAST
#include<Eigen/Eigen>
#include<vector>
#include"Triangle.h"
#include"Window.h"
#include"Camera.h"
#include"Shader.h"
#include<functional>
#include<optional>
#include"Texture.h"
namespace rst {
	struct pos_buf_id{
		int pos_id;
	};
	struct ind_buf_id {
		int ind_id;
	};
	struct col_buf_id {
		int color_id;
	};
	enum class Buffers {
		frame=1,
		depth=2
	}; 
	inline Buffers operator|(Buffers a, Buffers b) {
		return Buffers((int)a | (int)b);
	}
	inline Buffers operator&(Buffers a, Buffers b) {
		return Buffers((int)a & (int)b);
	}
	class Rasterizer {
	typedef std::function<Eigen::Vector3f(vertex_shader_payload)> VS;
	typedef std::function<Eigen::Vector3f(fragment_shader_payload)> FS;
	public:
		Rasterizer(Window w,Camera c);
		Rasterizer(Window w, Camera c, VS vs,FS fs):Rasterizer(w,c){
			vertex_shader = vs;
			fragment_shader = fs;
		}
		Eigen::Matrix4f get_mvp();
		void draw(std::vector<Triangle*>& t ,int i);
		void draw(const std::vector<Triangle*>& t);
		void rasterize_triangle(Triangle& t);
		void rasterize_triangle(Triangle& t, std::array<Eigen::Vector3f, 3> pos);
		void clear(Buffers buf);
		void set_model_matrix(Eigen::Matrix4f m);
		void set_view_matrix(Eigen::Matrix4f m);
		void set_projection_matrix(Eigen::Matrix4f m);
		void set_texture(Texture tex) {
			texture = tex;
		}
		void set_fragment_shader(FS fs){ fragment_shader=fs; }
		void set_vertext_shader(VS vs) { vertex_shader = vs; }
		int get_index(float x, float y) {
			int id= x + window.width * (window.height - 1 - y);
			if (id >= 0)	return id;
			return 0;
		}

		//buf
		std::vector<Eigen::Vector3f> frame_buf;
		std::vector<float> depth_buf;
		void set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color);
	private:									
		//mvp									
		Eigen::Matrix4f model;					
		Eigen::Matrix4f view;					
		Eigen::Matrix4f projection;
		
		//window
		Window window;
		//Camera
		Camera camera;
		//buffer
		std::map<int,std::vector<Eigen::Vector3f>> pos_buff;
		std::map<int,std::vector<Eigen::Vector3f>> ind_buff;
		std::map<int,std::vector<Eigen::Vector3f>> col_buff;
		std::map<int,std::vector<Eigen::Vector3f>> nor_buff;
		int next_id = 0;
		//shader
		VS vertex_shader;
		FS fragment_shader;
		//texture
		std::optional<Texture> texture;

	};
}



#endif
