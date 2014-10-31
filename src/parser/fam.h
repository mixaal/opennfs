#ifndef _FAM_H_
#define _FAM_H_

#include <string>
#include <string.h>
#include "nfs_io.h"
#include <set>

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <map>
#include <assert.h>
#include "bitmap.h"

namespace nfs {
	namespace fam {

		/**
		 * @brief determines the polygon type fam file mapping
		 */
		enum PolygonType
		{
			UNKNOWN = 0,
			QUAD2 = 0x4,
			TRIANGLE = 0x83,
			QUAD = 0x84,
			UNTEXTUREDQUAD = 0x8C,
			TRIANGLE2 = 0x8B
		};

		class info {
			public:
				info(std::string _namespace, std::string _name, std::string _value) :
					_namespace(_namespace),
					_name(_name),
					_value(_value) {
				}

				void set_value(std::string _val) {	
					_value = _val;
				}

				std::string name_space() {
					return _namespace;
				}
				std::string name() {
					return _name;
				}
				std::string value() {
					return _value;
				}
			private:
				std::string _namespace;
				std::string _name;
				std::string _value;
				
		};

		/**
		 * @brief typical texture uvmap coordinates in wavefront obj file
		 */
		class obj_vt {
			public:
				obj_vt(float u, float v) : _u(u), _v(v) {
				}

				std::string to_string() {
					return "vt " + std::to_string(_u) + " " + std::to_string(_v);
				}
				u_int32_t index() {
					return _index;
				}
				void set_index(u_int32_t index){
					_index = index;
				}
				void set_info(std::string _namespace, std::string _name, std::string _value){
					_info = new info(_namespace, _name, _value);
					printf("WANT: this=%p set_info(): _info=%p\n", this, _info);
				}
				info *get_info() {
					printf("WANT: this=%p get_info(): _info=%p\n", this, _info);
					return _info;
				}
				bool has_info() {
					return _info != NULL;
				}
			private:
				float _u, _v;
				u_int32_t _index;
				info *_info = NULL;
			
		};

		/**
		 * @brief need for speed texture coordinates to wavefront texture coordinates converter class 
		 */
		class uvmap {
			public:
				uvmap(int32_t _u, int32_t _v) : _u(_u), _v(_v) {
				}
				int32_t u() { return _u ; }
				int32_t v() { return _v ; }
				obj_vt *normalize(u_int32_t _xp, u_int32_t _yp, u_int32_t w, u_int32_t h) {
					printf("_normalize: _u=%u _v=%u _xp=%u _yp=%u w=%u h=%u\n", _u, _v, _xp, _yp, w, h);
					_nu = (float) (_xp+_u )/ w;
					_nv = 1.0f - (float) (_yp+_v) / h;
					printf("_normalize: _nu = %f _nv = %f\n", _nu, _nv);
					return new obj_vt(_nu, _nv);
				}
				
			private:
				int32_t _u, _v;
				float _nu, _nv;
		};

		class vertex {
			public:
				vertex(int32_t _x, int32_t _y, int32_t _z) : _x(_x), _y(_y), _z(_z) {
				}
				int32_t x() { return _x ; }
				int32_t y() { return _y ; }
				int32_t z() { return _z ; }
				std::string to_string() {
					return "v " + std::to_string(_x)  + " " + std::to_string(_y) + " "  + std::to_string(_z);
				}
				std::string to_float(float scale) {
					return "v " + std::to_string(_x*scale)  + " " + std::to_string(_y*scale) + " "  + std::to_string(_z*scale);
				}
			private:
				int32_t _x,_y,_z;
		};
                class face_or_material {
		// placeholder for face or material
			public:
				face_or_material() {
					_infos.clear();
				}
				virtual void to_obj() = 0;
				virtual std::string to_string() = 0;
				virtual std::vector<info*> get_infos() {
					return _infos;
				}
				virtual void add_info(info *_info) {
					_infos.push_back(_info);
				}
			protected:
				std::vector<info*> _infos;
				
                };
		class face : public face_or_material {
			public:
				face(
					u_int32_t i1, u_int32_t i2, u_int32_t i3,
					obj_vt *t1, obj_vt *t2, obj_vt *t3
				) :    face_or_material(),
					_i1(i1), _i2(i2), _i3(i3) ,
					_t1(t1), _t2(t2), _t3(t3) 
					{  }
				u_int32_t i1() { return _i1; }
				u_int32_t i2() { return _i2; }
				u_int32_t i3() { return _i3; }
				virtual void to_obj() {
					++_i1; 
					++_i2;
					++_i3;
					++_ti1; 
					++_ti2;
					++_ti3;
				}
				static void reset() { _idx = 0; }
				virtual std::string to_string() {
					_t1->set_index(3*_idx+1);
					_t2->set_index(3*_idx+2);
					_t3->set_index(3*_idx+3);

					if(_t1->has_info()){
						_t1->get_info()->set_value(std::to_string(_t1->index()));
						add_info(_t1->get_info());
					}
					if(_t2->has_info()){
						_t2->get_info()->set_value(std::to_string(_t2->index()));
						add_info(_t2->get_info());	
					}
					if(_t3->has_info()){
						_t3->get_info()->set_value(std::to_string(_t3->index()));
						add_info(_t3->get_info());
					}

					std::string result = "f " +
						std::to_string(_i1) +"/" + std::to_string(_t1->index()) + " "+
						std::to_string(_i2) +"/" + std::to_string(_t2->index()) + " "+
						std::to_string(_i3) +"/" + std::to_string(_t3->index()) + " ";
					++_idx;
					return result;
				}

				std::string serialize_tex_coords() {
					return 
						_t1->to_string() + "\n" +
  						_t2->to_string() + "\n" +
						_t3->to_string();
				}
			private:
				u_int32_t _i1, _i2, _i3;
				u_int32_t _ti1, _ti2, _ti3;
				obj_vt *_t1, *_t2, *_t3;
				static u_int32_t _idx;
		};


		class material : public face_or_material {
			public:
				material(bitmap *bm) : face_or_material(), _bm(bm) { }
				material(bitmap *bm, std::string alt_name) : face_or_material(), _bm(bm) { 
					_alt_name = alt_name;
				}
				virtual void to_obj() {
				}
				virtual std::string to_string() {
					return _bm->to_string();
				}
				std::string to_mtllib() {
					std::string mat_name = (_alt_name != "") ? _alt_name : _bm->name();
					return 
						"newmtl " + mat_name + "\n" +
						"Kd 1.0 1.0 1.0\n" +
						"Ka 0.2 0.2 0.2\n" +
						"Ks 0.2 0.2 0.2\n" +
						"Ns 128\n" +
						"map_Kd " + _bm->png_name() + "\n";
				}
				
			private:
				bitmap *_bm;
				std::string _alt_name;
		};



		class generic_chunk {
			public:
				generic_chunk(nfs::fstream *file) : _file(file->clone()) { }
				virtual void parse() = 0;
				virtual void serialize() = 0;
				void check_id(std::string id);
				void append_info(
					std::string fname, 
					std::string nspace, 
					std::string vname, 
					std::string value
				);
				void append_info(
					std::string fname,
					std::vector<info*> _infos
				);
				void skip_bytes(size_t nbytes);
			protected:
				nfs::fstream *_file;
				u_int32_t get_uint32();
				u_int16_t get_uint16();
				u_int8_t get_uint8();
				void close();
				std::string get_str(size_t len);


		};

		class shpi_chunk : public generic_chunk {
			public:
				shpi_chunk(nfs::fstream *file) : generic_chunk(file) { }
				void add_bitmap(bitmap *b);
				std::vector<bitmap *> get_bitmaps();
				bitmap *get_bitmap_by_name(std::string name);
				void parse() ;
				void serialize();
				void serialize_tires();
				bitmap *get_final_bitmap() { _final_bitmap->set_name(_final_bitmap_name); return _final_bitmap; }
				bitmap *create_final_bitmap();
				void set_final_bitmap_name(std::string _name) {
					_final_bitmap_name = _name;
					_final_bitmap->set_name(_name);
				}
				int to_png(bitmap *palette, bitmap *texture, std::string prefix);
			private:
				std::string _name="";
				bool _is_tyre(bitmap *b);
				bitmap *get_palette();
				std::multiset<bitmap *, compare_two_bitmaps> _bitmaps;
				std::vector<bitmap *> _bitmap_vector;
				bitmap *_final_bitmap;
				std::string _final_bitmap_name = "";
		};


		class wwww_chunk : public generic_chunk {
			public:
				wwww_chunk(nfs::fstream *file) : generic_chunk(file) { }
				void parse() ;
				void serialize();
				std::vector<bitmap *> get_bitmaps();
				wwww_chunk *get_wwww_chunk_at(size_t index);
				shpi_chunk *get_shpi_at(size_t shpi_no);
				size_t number_of_shpi_chunks();
			private:
				void _add(generic_chunk *chunk);
				void _add_shpi_chunk(shpi_chunk * chunk);
				std::vector<u_int32_t> chunk_offset;
				std::vector<generic_chunk *> _chunks;
				std::vector<shpi_chunk*> _shpi_chunks;
				std::vector<bitmap *> _bitmaps;
				void push_back_bitmaps(shpi_chunk *shpi);
		};



		class orip_chunk : public generic_chunk {
			public:
				orip_chunk(nfs::fstream *file, shpi_chunk *shpi) : generic_chunk(file), _shpi(shpi) { }
				void add_vertex(u_int32_t x, u_int32_t y, u_int32_t z);
				void add_face(
					u_int32_t i1, u_int32_t i2, u_int32_t i3, 
					obj_vt *t1, obj_vt *t2, obj_vt *t3
				);
				void add_material(bitmap *bm);
				void add_tcoord(u_int32_t u, u_int32_t v);
				void parse() ;
				/**
				 * @brief serialize the 3D model represented by ORIP chunk
				 */
				void serialize();
			private:
				bool _is_tyre(u_int8_t texno) {
					std::string tname = texture_lookup.at(texno);
					return tname == "rertires" || tname == "frntires";
				}
				std::string _tire(u_int8_t texno) {
					return (_is_tyre(texno)) ? texture_lookup.at(texno) : NULL;
				}
				obj_vt * _normalize(u_int32_t _idx, bitmap *_texture, bitmap *final_bitmap);
				std::string _name;
				shpi_chunk *_shpi;
				std::vector<vertex> _vertices;
				std::vector<uvmap>  _tcoords;
				std::vector<face_or_material*>   _face_or_material;
				std::vector<std::string> texture_lookup;
				bool is_quad(u_int8_t p_type) {
					return p_type == PolygonType::QUAD2 || p_type == PolygonType::QUAD;
				}
				std::string _get_texture_name(u_int8_t texno) {
					return (_is_tyre(texno)) ? "tyr1" : texture_lookup.at(texno);
				}

		};

		class parser {
			public:
				/*
				 * @brief the main fam file parser method
				 * @param path fam filename path
				 */
				parser(std::string path) : path(path) { }
				/*
				 * @brief start the parsing
				 */
				wwww_chunk *start();
				std::vector<bitmap *> get_bitmaps(wwww_chunk *wwww);
			private:
				std::string path;
		};


		class track_segment {
			public:
				track_segment(size_t no) : g_no(no) {
				}

				void add_texture(u_int8_t tex_no) {
					_textures.push_back(tex_no);
				}

				u_int8_t get_texture(u_int8_t tex_no) {
					return _textures.at(tex_no);
				}

				std::string to_string(size_t i1, size_t i2, size_t i3, bool left_bottom) {
					if(left_bottom) {
					return "f " + 
						std::to_string(1+i1) + "/1/1 " +
						std::to_string(1+i2) + "/2/1 " +
						std::to_string(1+i3) + "/4/1 ";
					}
					else {
					return "f " + 
						std::to_string(1+i1) + "/4/1 " +
						std::to_string(1+i2) + "/2/1 " +
						std::to_string(1+i3) + "/3/1 ";
					}
				}

				size_t Xn(size_t prio, size_t n) {
					if(n>=0 && n<=10)
						return g_no*88 + prio*11 + n;
					else 
						throw std::runtime_error("index out of range!");
				}
	
				std::vector<vertex*> _A;
				std::vector<vertex*> _B;
				std::vector<vertex*> _C;
				std::vector<vertex*> _D;
				std::vector<vertex*> _E;

			private:
				size_t g_no;
				std::vector<u_int8_t> _textures;

		};

		class trkd_chunk : public generic_chunk {
			public:
				trkd_chunk(nfs::fstream *file, wwww_chunk *wwww) : generic_chunk(file) {
					_wwww_chunk = wwww;
				}
				void parse();
				void serialize();
				bool closed_road();
				bitmap *fetch_texture(size_t chunkno);
				bitmap * fetch_palette(size_t tex_no);
			private:
				void parse_track_segment_strip(std::vector<vertex*> *_X);
				void serialize_track_segment_strip_into_verices(std::vector<vertex*> *_X);
				void prepare_segments_for_faces(track_segment *segment) ;
				void prepare_texture();
				void serialize_track_into_faces();
				void parse_track_segment(size_t no);
				void serialize_track_segment(track_segment *segment);
				std::vector<track_segment*> _segments;
				std::map<u_int8_t, std::vector<std::string>> _textured_segments;
				std::map<u_int8_t, bitmap *> texture_bitmap;
				bitmap *_final_texture;
				wwww_chunk *_wwww_chunk;

		};



	}

}

#endif /* _FAM_H_ */
