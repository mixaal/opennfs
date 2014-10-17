/*
 * material.h
 *
 *  Created on: 3. 11. 2013
 *      Author: mikc
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <map>
#include <opennfs/opengl.h>

namespace visualizer {
#define TX_MAX_SZ 1024
class Material {
public:
	Material() {
		nexp = 128;
		Ka[0] = Ka[1] = Ka[2] = Ka[3] = 1.0f;
		Kd[0] = Kd[1] = Kd[2] = Kd[3] = 1.0f;
		Ks[0] = Ks[1] = Ks[2] = Ks[3] = 0.0f;
		texno = 0;
	}
	//static bool has_texture(std::string texname);
	//static int get_texture(std::string texname);
	void add_texture(std::string texname);
	void ambient(float r, float g, float b, float a=1.0f);
	void diffuse(float r, float g, float b, float a=1.0f);
	void specular(float r, float g, float b, float a=1.0f);
	void shiness(float Nexp);
	void draw();
	void gauss_texture(float r, float g, float b, float a=1.0f, size_t width=64);
	void wyvill_texture(float r, float g, float b, float a=1.0f, size_t width=64);
private:
	float nexp;
	float Ka[4];
	float Kd[4];
	float Ks[4];
	GLuint texno;
	static std::map<std::string, GLuint> textures;
};

};


#endif /* MATERIAL_H_ */
