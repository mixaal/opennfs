#ifndef _CLOUD_H_
#define _CLOUD_H_

#include <math.h>
#include <vector>
#include <string>

#include <algorithm>

#include <opennfs/material.h>
#include <opennfs/matrix.h>
#include <opennfs/billboard.h>
#include <opennfs/types.h>

namespace visualizer {

namespace types {

	typedef struct {
		XYZ p;
		XYZ delta;
		XYZ psort;
		double r;
		double h;
		double c,a;
		int cnt;
		int top;
		int draw;
		int surf;
		double vapor;
	} voxel_t;

}

#define CLOUD_MAX_LAYER_HEIGHT 20
#define CLOUD_SURF_NORMAL 8
#define CLOUD_SURF_BOTTOM_LAYER 4


class CloudLayer {
public:
	CloudLayer(std::string layer_definition_file, float altitude = 0.0f, float scale = 1.0f) :
		altitude(altitude), scale(scale) {
		material = new Material();
		base = new Material();
		lightmap = new Material();

		read_cloud_map(layer_definition_file);
		generate();
		optimize();

		sun_blue = sun_green = sun_red = 1.0f;
		//material->add_texture("cumulus2.png");
		//base->add_texture("cumulus2.png");

		//material->gauss_texture(0.7f, 0.9f, 0.8f);
		//base->gauss_texture(0.5f, 0.5f, 0.5f);

		material->wyvill_texture(1.0f, 1.0f, 1.0f);
		base->wyvill_texture(1.0f, 1.0f, 1.0f);

		m = new Matrix();
		position(0.0f, altitude, 0.0f);
		set_observer_position(0.0f, 0.0f, 0.0f);
	}
	void draw(float position[3]);
	void set_sun_color(float color[3]);
	void set_sun_position(float azimuth, float elevation);
	void set_dissipation(float a);
	void evolve(int hour, int minute);
	void position(float x, float y, float z);
	void set_observer_position(float x, float y, float z);
	void set_sun_position(float x, float y, float z);
	void light_shafts();

private:
	float altitude;
	float scale;
	float albedo = 0.9f;
	float extinction = 8.0f;
	std::vector<unsigned char> cloud_mass;
	void read_cloud_map(std::string filename);
        void generate();
	void optimize();
	void drawVoxel(types::voxel_t *v, Billboard* b) ;
	float phase_function(types::XYZ p, types::XYZ *delta);
	float rayleigh(float a); // phase function
	float henyey_greenstein(float a, float g=0.99f); //phase function
	float color(float a, float Ik);
	size_t cloud_map_width;
	types::voxel_t ***cloud3d;
	float get_mass(size_t i, size_t j);
	float max_vapor = 0;
	bool remove_cloud_cell(ssize_t i, ssize_t j, ssize_t k);
	types::XYZ project(float x, float y, float z);
	types::UV uv(types::XYZ P);
	void render_plate(types::XYZ A, types::XYZ B, types::XYZ C, types::XYZ D, float v);
	void render_shell(float distance, float angle1, float angle2);
	std::vector<types::voxel_t*> voxels;
	std::vector<types::voxel_t*> basement;
	float blend_factor_from_vapor(float vapor, float a);
	float dissipation=0.01f;

	float sun_red, sun_green, sun_blue;

	float eye_x, eye_y, eye_z;
	float sun_x, sun_y, sun_z, sun_azimuth, sun_elevation;
	float cloud_x, cloud_y, cloud_z;

	Material *material, *base, *lightmap;
	Matrix *m;


	struct sort_voxels
	{
		// sortstruct needs to know its containing object
		CloudLayer *cloud;
		sort_voxels(CloudLayer* c) : cloud(c) {};
 
		// this is our sort function, which makes use
		// of some non-static data (sortascending)
		bool operator() ( types::voxel_t *a, types::voxel_t *b)
		{
			types::XYZ pa = a->p;
			types::XYZ pb = b->p;
	
			pa.x += cloud->cloud_x - cloud->eye_x;
			pa.y += cloud->cloud_y - cloud->eye_y;
			pa.z += cloud->cloud_z - cloud->eye_z;

			pb.x += cloud->cloud_x - cloud->eye_x;
			pb.y += cloud->cloud_y - cloud->eye_y;
			pb.z += cloud->cloud_z - cloud->eye_z;

			float d1 = pa.x * pa.x + pa.y * pa.y + pa.z * pa.z;
			float d2 = pb.x * pb.x + pb.y * pb.y + pb.z * pb.z;

			return d1 > d2;
		}
	};

};

}


#endif /* _CLOUD_H_ */
