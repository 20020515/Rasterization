#include"Shader.h"
Eigen::Vector3f Shader::phong_fragment_shader(const fragment_shader_payload& payload) {
	//ka,kd,ks
	//环境光
	Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
	Eigen::Vector3f am_light{ 10,10,10 };
	//漫反射
	Eigen::Vector3f kd = payload.color;
	//高光
	Eigen::Vector3f ks = Eigen::Vector3f(0.8, 0.8, 0.8);
	//lights
	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };
	light lights[]={
		l1,l2
	};
	//param
	auto point = payload.shader_coords;
	auto normal = payload.normarl;
	Eigen::Vector3f shader_color = { 0,0,0 };
	int p = 100;
	for (const auto& light : lights) {
		//amb:
		Eigen::Vector3f La = ka.cwiseProduct(am_light);
		//diffuse:
		double r_2 = (light.position - point).dot(light.position - point);
		Eigen::Vector3f I = (light.position - point).normalized();
		Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r_2) * std::max((float)0, payload.normarl.normalized().dot(I));
		//special:
		Eigen::Vector3f V = (payload.eye_pos - point).normalized();
		Eigen::Vector3f h = (V + I).normalized();
		Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r_2) * std::pow(std::max((float)0, payload.normarl.normalized().dot(h)), p);
		shader_color += La + Ld + Ls;
	}
	return shader_color * 255.0f;
}
Eigen::Vector3f Shader::texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = { 0, 0, 0 };
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        return_color = payload.texture->getColor(payload.uv.x(), payload.uv.y());

    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{ {20, 20, 20}, {500, 500, 500} };//光照的位置和强度
    auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

    std::vector<light> lights = { l1, l2 };
    Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
    Eigen::Vector3f eye_pos{ 0, 0, 10 };

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.shader_coords;
    Eigen::Vector3f normal = payload.normarl;

    Eigen::Vector3f result_color = { 0, 0, 0 };

    for (auto& light : lights)
    {

        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        //ambient:
        Eigen::Vector3f V = (eye_pos - point).normalized();
        Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
        //diffuse:
        Eigen::Vector3f I = (light.position - point).normalized();
        double r_2 = (light.position - point).dot(light.position - point);
        Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r_2) * std::max((float)0, payload.normarl.normalized().dot(I));
        //specular
        Eigen::Vector3f h = (V + I).normalized();
        Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r_2) * std::pow(std::max((float)0, payload.normarl.normalized().dot(h)), p);
        result_color += La + Ld + Ls;
    }
    //std::cout << result_color << std::endl << std::endl;
    return result_color * 255.f;
}
Eigen::Vector3f Shader::vertex_shader(const vertex_shader_payload& payload)
{
	return Eigen::Vector3f();
}
