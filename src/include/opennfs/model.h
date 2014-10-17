/*
 * model.h
 *
 *  Created on: 12. 10. 2013
 *      Author: mikc
 */

#ifndef MODEL_H_
#define MODEL_H_


#include <string>
#include <vector>
#include <math.h>
#include <opennfs/material.h>
#include <opennfs/matrix.h>

namespace visualizer {


class Model {
private:
	int id;
	std::string fileName;
	Material *material;
	std::vector<float> _vertices;
	std::vector<float> _distance_from_vector;
	std::vector<float> _normals;
	std::vector<float> _texcoords;
	std::vector<unsigned int> _indices;
	std::vector<Matrix*> _matrices;
	std::vector<float> _colors;

	void load();
	void loadMtl(std::string file);
        bool light_enabled;
	float scale;



public:

	Model(int id, std::string fileName, int instanceno = 1, float scale = 1.0f) :
			id(id), fileName(fileName), scale(scale) {
		material = new Material();
		load();
		addInstances(instanceno);
                light_enabled = true;
	}
    
	void compute_distance(float x, float y, float z);
        void disable_light();
        void enable_light();

	void addInstances(int instanceno);
	void positionInstance(int idx, float x, float y, float z);
	void getInstancePosition(int idx, float *x, float *y, float *z);
	void rotateInstance(int idx, float x, float y, float z, float angle);

	void set_color_based_on_distance(
		float position[3], /* position */
		float start_color[2],  /* start color */
		float end_color[3]   /* end color */
	);
	void draw();
	void draw_towards(float eye_x, float eye_y, float eye_z);
	int getId();
};

};

#endif /* MODEL_H_ */
