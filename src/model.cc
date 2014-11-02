/*
 * model.cc
 *
 *  Created on: 12. 10. 2013
 *      Author: mikc
 */

#include <opennfs/model.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <opennfs/tools.h>
#include <string.h>
#include <opennfs/opengl.h>
#include <math.h>
#include <limits>
#include <opennfs/billboard.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

namespace visualizer {

int Model::getId() {
	return id;
}

void Model::enable_light()
{
	light_enabled = true;
}

void Model::disable_light()
{
	light_enabled = false;
}

void Model::compute_distance(float x, float y, float z)
{
	float 
		max_dist=std::numeric_limits<float>::min(), 
		min_dist=std::numeric_limits<float>::max();
	_distance_from_vector.clear();

	for(size_t idx = 0; idx<_vertices.size(); idx+=3) {
		float dx = _vertices.at(idx)   - x;
		float dy = 0 - y;
		float dz = _vertices.at(idx+2) - z;

		float d= sqrt(dx*dx + dy*dy + dz*dz);
		if(d>max_dist) max_dist=d;
		if(d<min_dist) min_dist=d;

		_distance_from_vector.push_back(d);
	}

	for(size_t idx =0 ; idx < _distance_from_vector.size(); idx++ ) {
		float delta = max_dist - min_dist;
		float normalized_dist = 
			( _distance_from_vector.at(idx) - min_dist ) / delta;
		_distance_from_vector.at(idx) =  normalized_dist;
	}
}

void Model::set_color_based_on_distance(
		float position[3], /* position */
		float start_color[2],  /* start color */
		float end_color[3]   /* end color */
)
{
	_colors.clear();
	compute_distance(position[0], position[1], position[2]);
	
	float rs = start_color[0];
	float gs = start_color[1];
	float bs = start_color[2];

	float re = end_color[0];
	float ge = end_color[1];
	float be = end_color[2];

	float dr = re - rs;
	float dg = ge - gs;
	float db = be - bs;

	for(size_t idx =0 ; idx < _distance_from_vector.size(); idx++ ) {
		float d = _distance_from_vector.at(idx);
		float r = rs + dr * d;
		float g = gs + dg * d;
		float b = bs + db * d;
		printf("idx=%u distance = %f c=[%f %f %f]\n", idx, d, r, g, b);
		_colors.push_back(r);
		_colors.push_back(g);
		_colors.push_back(b);
	}
}

void Model::addInstances(int instanceno) {
	for(int i=0; i<instanceno; i++) {
		Matrix *m = new Matrix();
		_matrices.push_back(m);
	}
}

void Model::getInstancePosition(int idx, float *x, float *y, float *z)
{
	float *m = _matrices.at(idx)->get();
	*x = m[12];
	*y = m[13];
	*z = m[14];
}

void Model::positionInstance(int idx, float x, float y, float z) {
	Matrix *m = _matrices.at(idx);
	m->position(x,y,z);
}

/*
 * 0:x^2(1-c)+c     4:xy(1-c)-zs     8:xz(1-c)+ys     0
   1:yx(1-c)+zs     5:y^2(1-c)+c     9:yz(1-c)-xs     0
   2:xz(1-c)-ys     6:yz(1-c)+xs     10:z^2(1-c)+c     0
        0              0               0        1
 *
 */
void Model::rotateInstance(int idx, float x, float y, float z, float angle) {
	Matrix *m = _matrices.at(idx);
	m->rotate(x, y, z, angle);
}


void Model::draw_towards(float eye_x, float eye_y, float eye_z)
{
	if (light_enabled) {
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
	}
	material->draw();
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glEnableClientState (GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, _vertices.data());
	if(light_enabled) {
		glNormalPointer(GL_FLOAT, 0, _normals.data());
	}
	else {
		glColorPointer(3, GL_FLOAT, 0, _colors.data());
	}
	float camera[3] = {eye_x, eye_y, eye_z};
	Billboard billboard(camera);
	glTexCoordPointer(2, GL_FLOAT, 0, _texcoords.data());
	for (int idx = 0; idx < _matrices.size(); idx++) {
		float obj_x, obj_y, obj_z;
		getInstancePosition(idx, &obj_x, &obj_y, &obj_z);
		glPushMatrix();
		glMultMatrixf(_matrices.at(idx)->get());
		billboard.sphericalBegin(
			obj_x, obj_y, obj_z
		);
		//glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT,
		//		_indices.data());
		glDrawArrays(GL_TRIANGLES, 0, _vertices.size()/3);
		billboard.end();
		glPopMatrix();
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LIGHTING);

}

#if 0
void Model::drawModel() 
{
		glBegin(GL_TRIANGLES);
		for(int i=0; i<_indices.size(); i++) {
			int idx = _indices.at(i);
			float u = _texcoords.at(2*idx);
			float v = _texcoords.at(2*idx+1);

			float x = _vertices.at(3*idx);
			float y = _vertices.at(3*idx+1);
			float z = _vertices.at(3*idx+2);

			float nx = _normals.at(3*idx);
			float ny = _normals.at(3*idx+1);
			float nz = _normals.at(3*idx+2);

			glNormal3f(nx, ny, nz);
			glTexCoord2f(u, v);
			glVertex3f(x, y, z);

			//std::cout << "Draw: T"<< idx << ": [" << u << ", " << v << "]" << std::endl;
			//std::cout << "Draw: P"<< i << ": [" << x << ", " << y << ", " << z << "]" << std::endl;
		}
		glEnd();

}
#endif

void Model::draw() {
	if (light_enabled) {
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
	}
	material->draw();
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glEnableClientState (GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, _vertices.data());
	if(light_enabled) {
		glNormalPointer(GL_FLOAT, 0, _normals.data());
	}
	else {
		glColorPointer(3, GL_FLOAT, 0, _colors.data());
	}
	glTexCoordPointer(2, GL_FLOAT, 0, _texcoords.data());


	//std::cout << std::endl << std::endl;


	for (int idx = 0; idx < _matrices.size(); idx++) {
		glPushMatrix();
		glMultMatrixf(_matrices.at(idx)->get());
		//glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT,
		//		_indices.data());

		glDrawArrays(GL_TRIANGLES, 0, _vertices.size()/3);
		//drawModel();
		glPopMatrix();
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LIGHTING);
}

void Model::loadMtl(std::string mtlFileName) {
    tools::StringTools stool;
    std::string f_chdir = stool.file_in_dir(fileName, mtlFileName);
	std::ifstream mtl_file(f_chdir);
	std::string line;



    std::cout << "Matrial lib: " << f_chdir << std::endl;



	while(std::getline(mtl_file, line)) {
		//std::cout << "XXX0:" << line << " XXX0" << std::endl;
		line=stool.ltrim(line);
		//std::cout << "XXX1:" << line << " XXX1" << std::endl;
		if(line.empty()) {
			continue;
		}
		if(line[0]=='#') {
			continue;
		}
        if(stool.startswith(line, "newmtl ")) {

        }
		if(stool.startswith(line, "Ka ")) {
               std::vector<float> Ka = stool.toFloat(line, ' ', 1);
               assert(Ka.size()==3);
               std::cout << "Ka" << Ka[0] << Ka[1] << Ka[2] << std::endl;
               material->ambient(Ka[0], Ka[1], Ka[2]);
		}
		if(stool.startswith(line, "Kd ")) {
               std::vector<float> Kd = stool.toFloat(line, ' ', 1);
               assert(Kd.size()==3);
               std::cout << "Kd" << Kd[0] << Kd[1] << Kd[2] << std::endl;
               material->diffuse(Kd[0], Kd[1], Kd[2]);
        }
        if(stool.startswith(line, "Ks ")) {
               std::vector<float> Ks = stool.toFloat(line, ' ', 1);
               assert(Ks.size()==3);
               material->specular(Ks[0], Ks[1], Ks[2]);
        }
        if(stool.startswith(line, "Ns ")) {
               std::vector<float> Ns = stool.toFloat(line, ' ', 1);
               assert(Ns.size()==1);
               material->shiness(Ns[0]);
        }
        if(stool.startswith(line, "map_Kd ")) {
               std::vector<std::string> terms = stool.split(line);
               assert(terms.size()==2);
               std::string texName = terms[1];
               material->add_texture(stool.file_in_dir(fileName, texName));
        }
	}
}

void Model::load() {
	tools::StringTools stool;
	std::ifstream obj_file(fileName);
	std::string line;
	std::vector<float> __tmp_vertices;
	std::vector<float> __tmp_vertex_normals;
	std::vector<float> __tmp_vertex_texcoords;
	size_t lineno=0;
	while(std::getline(obj_file, line)) {
		lineno++;
		//std::cout << "XXX0:" << line << " XXX0" << std::endl;
		line=stool.ltrim(line);
		//std::cout << "XXX1:" << line << " XXX1" << std::endl;
		if(line.empty()) {
			continue;
		}
		if(line[0]=='#') {
			continue;
		}
		if(stool.startswith(line, "mtllib")) {
		    std::vector<std::string> terms = stool.split(line);
            assert(terms.size()==2);
            std::cout << "_____XXX1:" << terms[1] << std::endl;
		    loadMtl(terms[1]);
		}
		if(stool.startswith(line, "v ")) {
			std::vector<float> vtx = stool.toFloat(line, ' ', 1);
			assert(vtx.size()==3);
			//std::cout << "Vertex:|" << vtx.at(0) << "|" << vtx.at(1) << vtx.at(2) << std::endl;
			__tmp_vertices.push_back(scale * vtx.at(0));
			__tmp_vertices.push_back(scale * vtx.at(1));
			__tmp_vertices.push_back(scale * vtx.at(2));
		}
		if(stool.startswith(line, "vn ")) {
			std::vector<float> vtx = stool.toFloat(line, ' ', 1);
			assert(vtx.size()==3);
			//std::cout << "Vertexnormal:|" << vtx.at(0) << "|" << vtx.at(1) << vtx.at(2) << std::endl;
			__tmp_vertex_normals.push_back(vtx.at(0));
			__tmp_vertex_normals.push_back(vtx.at(1));
			__tmp_vertex_normals.push_back(vtx.at(2));
		}
		if(stool.startswith(line, "vt ")) {
					std::vector<float> vtx = stool.toFloat(line, ' ', 1);
					assert(vtx.size()==2);
					//std::cout << "VertexText:|" << vtx.at(0) << "|" << vtx.at(1) << std::endl;
					__tmp_vertex_texcoords.push_back(vtx.at(0));
					__tmp_vertex_texcoords.push_back(vtx.at(1));
		}
		if(stool.startswith(line, "f ")) {
			std::vector<std::string> _triangle_indices=stool.split(line);
 			if(_triangle_indices.size()!=4) {
				std::cerr << "Can't parse: " << line << "at: " << lineno << std::endl;
				std::cerr << "Triangle indices size: " << _triangle_indices.size() << std::endl;
				exit(1);
			}
			//assert(_triangle_indices.size()==4); // includes f
 	           for (int idx=1; idx<_triangle_indices.size();idx++) {
        	    	std::vector<float> vtn_idx=stool.toFloat(_triangle_indices.at(idx), '/');
	            	assert(vtn_idx.size()==3);
        	    	int v_i = vtn_idx.at(0) - 1;
         		int t_i = vtn_idx.at(1) - 1;
 	           	int n_i = vtn_idx.at(2) - 1;
        	    	assert(v_i >= 0);
	            	assert(t_i >= 0);
        	    	assert(n_i >= 0);

            		assert(1+2*t_i<__tmp_vertex_texcoords.size());
	            	assert(2+3*n_i<__tmp_vertex_normals.size());
	            	assert(2+3*v_i<__tmp_vertices.size());

			float x = __tmp_vertices.at(3*v_i);
			float y = __tmp_vertices.at(3*v_i+1);
			float z = __tmp_vertices.at(3*v_i+2);

			float nx = __tmp_vertex_normals.at(3*n_i);
			float ny = __tmp_vertex_normals.at(3*n_i+1);
			float nz = __tmp_vertex_normals.at(3*n_i+2);

			float tu = __tmp_vertex_texcoords.at(2*t_i);
			float tv = __tmp_vertex_texcoords.at(2*t_i+1);

			_vertices.push_back(x);
			_vertices.push_back(y);
			_vertices.push_back(z);
	
            		_normals.push_back(nx);
            		_normals.push_back(ny);
            		_normals.push_back(nz);

			_texcoords.push_back(tu);
			_texcoords.push_back(tv);

			//std::cout << "TEX COORDS: idx: " << v_i << "[" <<  _texcoords.at(2*v_i) << ", " << _texcoords.at(2*v_i+1) << "]" << std::endl;

	            }

		}
	}
	std::cout << "Vtx: " << _vertices.size() << " Normals: " << _normals.size() << " Tex sz: " << _texcoords.size() << std::endl;

	assert(_vertices.size() == _normals.size());
	assert(_vertices.size()/3 == _texcoords.size()/2);
	
}
};


