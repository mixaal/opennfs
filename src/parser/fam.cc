#include "fam.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <vector>
#include <assert.h>
#include <map>
#include "bitmap.h"
#include <sstream>
#include <iomanip>

namespace nfs {

namespace fam {

u_int32_t face::_idx = 0;

wwww_chunk * parser::start() {
	nfs::fstream *fam_file = new nfs::fstream(path, std::ios::in | std::ios::binary);
	//std::string id = fam_file->get_str(4);
	//printf("id=%s\n", id.c_str());
	wwww_chunk *wwww = new wwww_chunk(fam_file);
	wwww->parse();
	wwww->serialize();
	return wwww;
}

std::vector<bitmap*> parser::get_bitmaps(wwww_chunk *wwww)
{
	return wwww->get_bitmaps();
}

void generic_chunk::append_info(std::string _fname, std::string _namespace, std::string name, std::string value)
{
	
	nfs::fstream *car_info;
	car_info = new nfs::fstream(_fname + ".info", std::ios::out|std::ios::app);
	*car_info->get_stream() << _namespace << "." << name << "=" << value << std::endl;
	car_info->close();
}

void generic_chunk::append_info(std::string _fname, std::vector<info*> _infos)
{
	if (0 == _infos.size()) return;
	for(std::vector<info*>::iterator it=_infos.begin(); it != _infos.end(); ++it) {
		info *_info = *it;
		if(_info == NULL)
			return;
		append_info(_fname, _info->name_space(), _info->name(), _info->value());
	}
}

void generic_chunk::check_id(std::string id) {
	std::string chunk_id = _file->get_str(4);
	if (id != chunk_id) {
		fprintf(stderr, "expecting %s chunk at position: %lu\n", id.c_str(),
				_file->position);
		throw std::runtime_error("unexpected chunk");
	}
}

u_int32_t generic_chunk::get_uint32() {
	return _file->get_uint32();
}

u_int16_t generic_chunk::get_uint16() {
	return _file->get_uint16();
}

u_int8_t generic_chunk::get_uint8() {
	return _file->get_uint8();
}

std::string generic_chunk::get_str(size_t len) {
	return _file->get_str(len);
}

void generic_chunk::close() {
	_file->close();
}

void generic_chunk::skip_bytes(size_t nbytes) {
	_file->position += nbytes;
}

void wwww_chunk::_add(generic_chunk *chunk) {
	_chunks.push_back(chunk);
}


size_t wwww_chunk::number_of_shpi_chunks()
{
	return _shpi_chunks.size();
}

shpi_chunk *wwww_chunk::get_shpi_at(size_t shpi_no) {
	return _shpi_chunks.at(shpi_no);
}

void wwww_chunk::_add_shpi_chunk(shpi_chunk * chunk) {
	_shpi_chunks.push_back(chunk);
}

void wwww_chunk::push_back_bitmaps(shpi_chunk *shpi)
{
	std::vector<bitmap*> _bmaps;
	if(shpi==NULL) return;
	_bmaps = shpi->get_bitmaps();
	for(
		std::vector<bitmap*>::iterator it = _bmaps.begin();
		it != _bmaps.end();
		++it
	) {
		bitmap *b = *it;
		_bitmaps.push_back(b);
	}
	printf("push_back_bitmaps: shpi=%p bmaps.size()=%lu _bitmaps.size()=%lu\n", shpi, _bmaps.size(), _bitmaps.size());
}

std::vector<bitmap *> wwww_chunk::get_bitmaps()
{
	return _bitmaps;
}

wwww_chunk *wwww_chunk::get_wwww_chunk_at(size_t index)
{
	std::vector <wwww_chunk *> _wwww_chunks;
	for(
		std::vector<generic_chunk *>::iterator it = _chunks.begin();
		it != _chunks.end();
		++it
	) {
		//printf("PROCESSING\n");
		if (typeid(**it) == typeid(wwww_chunk)) {
			//printf("   + PROCESSING -> wwww chunk\n");
			wwww_chunk *wwww = dynamic_cast<wwww_chunk *>(*it);
			_wwww_chunks.push_back(wwww);
		}
	}

	size_t wwww_chunks_sz = _wwww_chunks.size();
	if (wwww_chunks_sz > index ) {
		return _wwww_chunks.at(index);
	}
	else {
		throw std::runtime_error("get_wwww_chunk_at(): index out of range");
	}
}



void wwww_chunk::parse() {
	//_bitmaps.clear();
	size_t wwww_offset = _file->position;
	check_id("wwww");
	u_int32_t chunk_no = _file->get_uint32();
	printf("wwww chunk at offset : %lu, holding: %u chunks\n", wwww_offset,
			chunk_no);
	size_t save_offset = _file->position;
	for (u_int32_t i = 0; i < chunk_no; i++) {
		_file->position = save_offset + i * sizeof(u_int32_t);
		size_t chunk_offset = _file->get_uint32();
		_file->position = wwww_offset + chunk_offset;
		std::string chunk_id = _file->get_str(4);
		_file->position -= 4; // go to the beginning of header
		if (chunk_id == "ORIP") {
			size_t save_orip_offset = _file->position;
			// After Orip, there must be SHPI
			_file->position = save_offset + (i + 1) * sizeof(u_int32_t);
			size_t orip_shpi_offset = _file->get_uint32();
			_file->position = wwww_offset + orip_shpi_offset;
			printf("EXPECTING SHPI @ %lx\n", _file->position);
			shpi_chunk *shpi = new shpi_chunk(_file);
			shpi->parse();
			// back to orip chunk
			_file->position = save_orip_offset;
			orip_chunk *orip = new orip_chunk(_file, shpi);
			_add(orip);
			orip->parse();
			_add(shpi);
		} else if (chunk_id == "wwww") {
			wwww_chunk *wwww = new wwww_chunk(_file);
			_add(wwww);
			wwww->parse();
		} else if (chunk_id == "SHPI") {
			shpi_chunk *shpi = new shpi_chunk(_file);
			_add(shpi);
			shpi->parse();
			_add_shpi_chunk(shpi);
			push_back_bitmaps(shpi);
		} else if (chunk_id == "TRKD") {
			trkd_chunk *trkd = new trkd_chunk(_file, NULL);
			trkd->parse();
		} else {
			fprintf(stderr, "Unknown chunk: %s at offset: %lu\n",
					chunk_id.c_str(), _file->position);
			throw std::runtime_error("unknown chunk");
		}
	}

}

void wwww_chunk::serialize() {
	for (std::vector<generic_chunk *>::iterator it = _chunks.begin();
			it != _chunks.end(); ++it) {
		generic_chunk *chunk = *it;
		chunk->serialize();
	}
}

void orip_chunk::add_vertex(u_int32_t x, u_int32_t y, u_int32_t z) {
	vertex v(x, y, z);
	_vertices.push_back(v);
}

void orip_chunk::add_tcoord(u_int32_t u, u_int32_t v) {
	uvmap uv(v, u);
	_tcoords.push_back(uv);
}

void orip_chunk::add_face(
	u_int32_t i1, u_int32_t i2, u_int32_t i3,
	obj_vt *t1, obj_vt *t2, obj_vt *t3
	) {
	face *f = new face(i1, i2, i3, t1, t2, t3);
	_face_or_material.push_back(f);

}

void orip_chunk::add_material(bitmap *bm) {
	material *m = new material(bm);
	_face_or_material.push_back(m);
}

obj_vt * orip_chunk::_normalize(u_int32_t _idx, bitmap *_texture, bitmap *_final_bitmap)
{
	try {
		u_int32_t x = _texture->xp();
		u_int32_t y = _texture->yp();
		u_int32_t w = _final_bitmap->width();
		u_int32_t h = _final_bitmap->height();
		std::cout << "XINDL: idx: " << _idx << " T: " << _texture->name() << " " << w << "x" << h << std::endl;
		// normalize the output according to the given texture size
		return _tcoords.at(_idx).normalize(x, y, w, h);
	}
	catch (std::out_of_range ex) {
		std::cout << "FIXME: got out of range here" << std::endl;
	}
	return NULL;
} 

void orip_chunk::parse() {
	size_t orip_offset = _file->position;
	check_id("ORIP");
	printf("Parsing ORIP at offset: 0x%08lx\n", _file->position);
	skip_bytes(12);
	u_int32_t vtx_no = get_uint32();
	skip_bytes(4);
	u_int32_t vertices = orip_offset + get_uint32();
	u_int32_t tex_no = get_uint32();
	u_int32_t tex_map = orip_offset + get_uint32();
	u_int32_t poly_no = get_uint32();
	u_int32_t poly_map = orip_offset + get_uint32();
	_name = get_str(12);
	u_int32_t tex_names_no = get_uint32();
	u_int32_t tex_names = orip_offset + get_uint32();
	skip_bytes(16);
	u_int32_t vtx_map = orip_offset + get_uint32();
	skip_bytes(8);
	u_int32_t wheel_no = get_uint32();
	u_int32_t wheels_offset = orip_offset + get_uint32();
	std::cout << "ORIP PARSE : " << _name << std::endl;
	std::cout << "  vtx no: " << std::to_string(vtx_no) << std::endl;
	std::cout << "  poly no: " << std::to_string(poly_no) << std::endl;
	
	assert( _shpi->get_final_bitmap() != NULL );

	bitmap *final_bitmap = _shpi->get_final_bitmap();
	_shpi->set_final_bitmap_name(_name);
	printf("XXX : %s\n", final_bitmap->name().c_str());
	add_material(final_bitmap);

	/* add vertices */
	for (u_int32_t i = 0; i < vtx_no; i++) {
		_file->position = vertices + 12 * i;
		add_vertex(get_uint32(), get_uint32(), get_uint32());
	}

	/* add texture coordinates */
	for (u_int32_t i = 0; i < tex_no; i++ ) {
		_file->position = tex_map + 8 * i;
		add_tcoord(get_uint32(), get_uint32());
	}

	/* make texture lookup table */
	texture_lookup.clear();
	for (u_int32_t i = 0; i < tex_names_no; i++) {
		_file->position = tex_names + 20 * i;
		std::string alt_name = get_str(8);
		std::string t_name = get_str(4);
		std::cout << "T_NAME: " << t_name << " ALT NAME: " << alt_name
				<< std::endl;
		if (t_name == "")
			texture_lookup.push_back(alt_name);
		else
			texture_lookup.push_back(t_name);
	}

	/* add faces aka polygons */
	for (u_int32_t i = 0; i < poly_no; i++) {
		_file->position = poly_map + 12 * i;
		u_int8_t p_type = get_uint8();
		printf("p_type=0x%02x\n", p_type);
		skip_bytes(1);
		u_int8_t tex_no = get_uint8();
		std::string t_name = _get_texture_name(tex_no);
		printf("MIMI TEXTURE: %u %s\n",tex_no, t_name.c_str());
		bitmap *texture = _shpi->get_bitmap_by_name(t_name);
		//add_material(texture);
		std::cout << "PARSE ORIP: USING TEXTURE: " << texture->to_string()
				<< std::endl;
		skip_bytes(1);
		u_int32_t pol1_off = 4 * get_uint32();
		u_int32_t pol2_off = 4 * get_uint32();
		printf("YYY: 0x%x %u %u\n", p_type, pol1_off, pol2_off);

		if (pol1_off != pol2_off && p_type == PolygonType::UNTEXTUREDQUAD) {
			p_type = PolygonType::QUAD;
		}
		if (pol1_off == pol2_off && is_quad(p_type)) {
			p_type = PolygonType::UNTEXTUREDQUAD;
		}

		// vertex index
		_file->position = vtx_map + pol1_off;
		u_int32_t v1 = get_uint32();
		u_int32_t v2 = get_uint32();
		u_int32_t v3 = get_uint32();
		u_int32_t v4 = get_uint32();

		// texture coordinates index
		_file->position = vtx_map + pol2_off;
		u_int32_t t1 = get_uint32();
		u_int32_t t2 = get_uint32();
		u_int32_t t3 = get_uint32();
		u_int32_t t4 = get_uint32();
		obj_vt *uv1, *uv2, *uv3, *uv4;

		printf("TEXZZZ %x: %s %u %u %u %u [%u %u %u %u]\n", p_type, texture->name().c_str(), texture->xp(), texture->yp(), texture->width(), texture->height(), t1, t2, t3, t4);

		switch (p_type) {
		case PolygonType::TRIANGLE:
		case PolygonType::TRIANGLE2:
			//FIXME: DO NOT NORMALIZE UV HERE
			// CREATE NEW UV and in serialize somehow make use of it
			// OTHERWISE WE REWRITE THE PIXEL WITH WRONG VALUES: E.g.:
			// in orig file _u=45, idx=0 [1]
			// here we normalize for two textures, the one which goes
			// the last will overwite the value for all previous one
			uv1 = _normalize(t1, texture, final_bitmap);
			uv2 = _normalize(t2, texture, final_bitmap);
			uv3 = _normalize(t3, texture, final_bitmap);
			//_normalize(t4, texture, final_bitmap);
			// HERE instead of t1, t2, t3, real [u, v] floats must go
			add_face(v1, v2, v3, uv1, uv2, uv3);
			break;
		case PolygonType::QUAD:
		case PolygonType::QUAD2:
			uv1 = _normalize(t1, texture, final_bitmap);
			uv2 = _normalize(t2, texture, final_bitmap);
			uv3 = _normalize(t3, texture, final_bitmap);
			uv4 = _normalize(t4, texture, final_bitmap);
			add_face(v1, v2, v3, uv1, uv2, uv3);
			add_face(v1, v3, v4, uv1, uv3, uv4);
			break;
		}

	}

	for (u_int32_t i = 0; i < poly_no; i++) {
		_file->position = poly_map + 12 * i;
		u_int8_t p_type = get_uint8();
		printf("p_type=0x%02x\n", p_type);
		skip_bytes(1);
		u_int8_t tex_no = get_uint8();
		std::string t_name = _get_texture_name(tex_no);
		bitmap *texture = _shpi->get_bitmap_by_name(t_name);
		//add_material(texture);
		std::cout << "PARSE ORIP: USING TEXTURE: " << texture->to_string()
				<< std::endl;
		skip_bytes(1);
		u_int32_t pol1_off = 4 * get_uint32();
		u_int32_t pol2_off = 4 * get_uint32();
		printf("YYY: 0x%x %u %u\n", p_type, pol1_off, pol2_off);

		if (pol1_off != pol2_off && p_type == PolygonType::UNTEXTUREDQUAD) {
			p_type = PolygonType::QUAD;
		}
		if (pol1_off == pol2_off && is_quad(p_type)) {
			p_type = PolygonType::UNTEXTUREDQUAD;
		}

		// vertex index
		_file->position = vtx_map + pol1_off;
		u_int32_t v1 = get_uint32();
		u_int32_t v2 = get_uint32();
		u_int32_t v3 = get_uint32();
		u_int32_t v4 = get_uint32();

		if(_is_tyre(tex_no)) {
			std::string tire_type = _tire(tex_no);
			append_info(_name, tire_type, "v1", std::to_string(v1));
			append_info(_name, tire_type, "v2", std::to_string(v2));
			append_info(_name, tire_type, "v3", std::to_string(v3));
			append_info(_name, tire_type, "v4", std::to_string(v4));
		}

		printf("TEXZZZ %x: %s %u %u %u %u\n", p_type, texture->name().c_str(), texture->xp(), texture->yp(), texture->width(), texture->height());

		switch (p_type) {
		case PolygonType::UNTEXTUREDQUAD:
			/* create untextured */
			size_t untextured_idx = _tcoords.size();
			add_tcoord(0, 0);
			add_tcoord(0, texture->width());
			add_tcoord(texture->height(), texture->width());
			add_tcoord(texture->height(), 0);
			printf("NORMALIZING UNTEXTURED=%lu [0,0]->[1,1] for %s\n", untextured_idx, texture->name().c_str());
			// normalize so it's in sync with the final texture
			obj_vt *t1 = _normalize(untextured_idx, texture, final_bitmap);
			obj_vt *t2 = _normalize(untextured_idx+1, texture, final_bitmap);
			obj_vt *t3 = _normalize(untextured_idx+2, texture, final_bitmap);
			obj_vt *t4 = _normalize(untextured_idx+3, texture, final_bitmap);

			// set tire markers
			if(_is_tyre(tex_no)) {	
				std::string tire_type = _tire(tex_no);
				t1->set_info(tire_type, "t1", "unknown");
				t2->set_info(tire_type, "t2", "unknown");
				t3->set_info(tire_type, "t3", "unknown");
				t4->set_info(tire_type, "t4", "unknown");
			}
			// add faces
			add_face(v1, v2, v3, t1, t2, t3);
			add_face(v1, v3, v4, t1, t3, t4);
			break;
		}

	}
}

void orip_chunk::serialize() {
	std::cout << "Serializing orip chunk: " << _name << std::endl;
	nfs::fstream *obj3d, *mtllib;
	obj3d = new nfs::fstream(_name + ".obj", std::ios::out);
	mtllib = new nfs::fstream(_name + ".mtl", std::ios::out);
	*obj3d->get_stream() << "mtllib " << _name << ".mtl" << std::endl;
	// serialize vertices
	for (std::vector<vertex>::iterator it = _vertices.begin();
			it != _vertices.end(); ++it) {
		vertex v = *it;
		*obj3d->get_stream() << v.to_string() << std::endl;
	}
	// serialize texture coordinates
	for (std::vector<face_or_material*>::iterator it =
			_face_or_material.begin(); it != _face_or_material.end(); ++it) {
		face_or_material *f = *it;
		if (typeid(face) == typeid(*f)) {
			face *_face = dynamic_cast<face *>(f);
			*obj3d->get_stream() << _face->serialize_tex_coords() << std::endl;
		}
	}
#if 0
	for (std::vector<uvmap>::iterator it = _tcoords.begin();
			it != _tcoords.end(); ++it) {
		uvmap uv = *it;
		obj3d->stream() << uv.to_string() << std::endl;
	}
#endif

	face::reset();
	for (std::vector<face_or_material*>::iterator it =
			_face_or_material.begin(); it != _face_or_material.end(); ++it) {
		face_or_material *f = *it;
		f->to_obj();
		if (typeid(material) == typeid(*f)) {
			material *m = dynamic_cast<material *>(f);
			*mtllib->get_stream() << m->to_mtllib() << std::endl;
		}
		*obj3d->get_stream() << f->to_string() << std::endl;
		append_info(_name, f->get_infos());
	}
	obj3d->close();
}

std::vector<bitmap *> shpi_chunk::get_bitmaps()
{
	_bitmap_vector.clear();
	for(std::multiset<bitmap*>::iterator it = _bitmaps.begin(); it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		_bitmap_vector.push_back(b);
	}
	return _bitmap_vector;
}

void shpi_chunk::parse() {
	static size_t shpi_chunk_no = 0;
	size_t shpi_offset = _file->position;
	check_id("SHPI");
	printf("SHPI chunk at offset:0x%lx\n", _file->position);
	_name = std::string("SHPI_") + std::to_string(_file->position);
	u_int32_t file_len = _file->get_uint32();
	u_int32_t bitmap_no = _file->get_uint32();
	std::string shpi_directory = _file->get_str(4); // directory identifier
	printf("SHPI PARSE: shpi_directory=%s\n", shpi_directory.c_str());

	for (u_int32_t i = 0; i < bitmap_no; i++) {
		std::string bitmap_name = _file->get_str(4);
		u_int32_t bitmap_offset = _file->get_uint32();
		bitmap *b = new bitmap(bitmap_name, _file, shpi_offset + bitmap_offset);
		b->parse();
		_bitmaps.insert(b);
	}

	_final_bitmap = create_final_bitmap();

#if 0
	for (std::priority_queue<bitmap *>::iterator it = _bitmaps.begin();
			it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		b->parse();
	}
#endif

}

bitmap *shpi_chunk::create_final_bitmap() {
	// walk the bitmaps from largest to smallest and create
        // the texture containing all of them
	std::vector<bitmap *> bmaps;
	printf("\tCreating final bitmap()\n");
	for(std::multiset<bitmap*>::iterator it = _bitmaps.begin(); it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		if (b->is_palette()) continue;
		bmaps.push_back(b);
	}
	return bitmap::create_final_bitmap(bmaps);
}

bool shpi_chunk::_is_tyre(bitmap *b)
{
	return b->name().find("tyr") == 0;
}

void shpi_chunk::serialize_tires() {
	std::string car_info = _final_bitmap_name ;

	bitmap *final_bitmap = get_final_bitmap();
	for(std::multiset<bitmap*>::iterator it = _bitmaps.begin(); it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		if (!_is_tyre(b))
			continue;
		append_info( car_info, b->name(), "xp", std::to_string(b->xp()) );
		append_info( car_info, b->name(), "yp", std::to_string(b->yp()) );
		append_info( car_info, b->name(), "width", std::to_string(b->width()) );
		append_info( car_info, b->name(), "height", std::to_string(b->height()) );
	}
	append_info( car_info, "final_texture", "width", std::to_string(final_bitmap->width()) );
	append_info( car_info, "final_texture", "height", std::to_string(final_bitmap->height()) );
}

void shpi_chunk::serialize() {
	printf("serializing shpi chunk: %s\n", _name.c_str());
	bitmap *palette = get_palette();
	if (!palette) {
		std::cerr << "Skipping chunk, no palette information found!"
				<< std::endl;
		return;
	}
	//FIXME: Remove this statement if you want to have separate bitmaps
	if(_final_bitmap_name == "" )
		_final_bitmap_name = _name;

	if(_final_bitmap_name != "") {
		printf("FINAL BITMAP: %s\n", _final_bitmap_name.c_str());
		bitmap *final_bitmap = get_final_bitmap();
		final_bitmap->equalize(palette);
		to_png(palette, final_bitmap, "");
		serialize_tires();
	}
	//else {
		for(std::multiset<bitmap*>::iterator it = _bitmaps.begin(); it != _bitmaps.end(); ++it) {
			bitmap *b = *it;
			printf("FINAL: %s %d [%u %u %u %u]\n", b->name().c_str(), b->processed, b->xp(), b->yp(), b->width(), b->height());
			to_png(palette, b, "");
		}
	//}
}

bitmap *shpi_chunk::get_bitmap_by_name(std::string name) {
	for (std::multiset<bitmap *>::iterator it = _bitmaps.begin();
			it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		if (b->name() == name) {
			return b;
		}
	}
	//throw std::runtime_error("bitmap " + name + " not found in shpi!");
	return new null_bitmap();

}

bitmap *shpi_chunk::get_palette() {
	for (std::multiset<bitmap *>::iterator it = _bitmaps.begin();
			it != _bitmaps.end(); ++it) {
		bitmap *b = *it;
		printf("GET PALETTE PROCESSING: %s [%u %u]\n", b->name().c_str(), b->width(), b->height());
		if (b->is_palette()) {
			return b;
		}
	}
	//throw std::runtime_error("no palette found in shpi!");
	return NULL;
}

int shpi_chunk::to_png(bitmap *palette, bitmap *texture, std::string prefix) {
	texture->set_png_prefix(prefix);
	return bitmap::to_png(palette, texture);
}

void trkd_chunk::parse_track_segment_strip(std::vector<vertex*> *_X) {
	std::vector<vertex*> _tmp;
	for(size_t idx=0; idx<=10; idx++) {
		_X->push_back(
			new vertex(
				get_uint32(),
				get_uint32(),
				get_uint32()
			)
		);
	}

}

void trkd_chunk::parse_track_segment(size_t trs_no) {
	size_t save_position = _file->position;
	check_id("TRKD");
	track_segment *segment = new track_segment(trs_no);
	u_int32_t record_len = get_uint32();
	u_int32_t record_no = get_uint32();

	std::cout << "Parsing track record no: " << record_no << " of length " << record_len << std::endl;
	skip_bytes(2);
	// textures
	for(int i=0; i<10; i++) {
		segment->add_texture(get_uint8());
	}
	skip_bytes(12); //refpoint

	skip_bytes(12);
	parse_track_segment_strip(&segment->_A);

	skip_bytes(12);
	parse_track_segment_strip(&segment->_B);

	skip_bytes(12);
	parse_track_segment_strip(&segment->_C);

	skip_bytes(12);
	parse_track_segment_strip(&segment->_D);

	skip_bytes(24);
	parse_track_segment_strip(&segment->_E);
	_segments.push_back(segment);

	skip_bytes(596);

}


bool trkd_chunk::closed_road() 
{
	std::cout << "closed_road(): " << _wwww_chunk->number_of_shpi_chunks() << std::endl;
	return 1==_wwww_chunk->number_of_shpi_chunks();
}

bitmap * trkd_chunk::fetch_palette(size_t tex_no)
{
		shpi_chunk *shpi;

		size_t chunk_no  = tex_no / 3;
	
		if(!closed_road()) {
			shpi = _wwww_chunk->get_shpi_at(chunk_no);
		}
		else {
			shpi = _wwww_chunk->get_shpi_at(0);
		}
		
		bitmap *pal = shpi->get_bitmap_by_name("!pal");
		return pal;
}

bitmap * trkd_chunk::fetch_texture(size_t tex_no)
{
		bitmap *texture;
		shpi_chunk *shpi;

		size_t chunk_no  = tex_no / 3;
		size_t idx = tex_no%3;
	
		std::vector<std::string> letters={"A", "B", "C"};
		if(!closed_road()) {
			std::string b_name = letters.at(idx) + "000";
			std::cout << "NEED BITMAP :" << b_name << " at chunk_no: " << chunk_no <<  std::endl;
			shpi = _wwww_chunk->get_shpi_at(chunk_no);
			texture = shpi->get_bitmap_by_name(b_name);
		}
		else {
			std::stringstream ss;
			ss << std::setw(2) << std::setfill('0') << chunk_no;
			std::string b_name = ss.str() + letters.at(idx) + "0";
			std::cout << "NEED BITMAP :" << b_name << " at chunk_no: " << chunk_no <<  std::endl;
			shpi = _wwww_chunk->get_shpi_at(0);
			texture = shpi->get_bitmap_by_name(b_name);
		}
		if(texture->is_null()) {
			std::cout << "WARNING: Cannot find this texture, want to find a different one." << std::endl;
			std::vector<bitmap *> bmaps = shpi->get_bitmaps();
			for(std::vector<bitmap*>::iterator it=bmaps.begin(); it!=bmaps.end(); ++it) {
				bitmap *b = *it;
				if(b->is_palette()) continue;
				texture = b;
				break;
			}
			
		}

		return texture;

}

void trkd_chunk::serialize_track_into_faces()
{

	nfs::fstream *track_file;
	nfs::fstream *track_mtl_file;
	track_file = new nfs::fstream(_file->path() + ".obj", std::ios::out|std::ios::app);
	track_mtl_file = new nfs::fstream(_file->path() + ".mtl", std::ios::out|std::ios::app);

	
	*track_file->get_stream() << "vt 0.0 0.0 " << std::endl;
	*track_file->get_stream() << "vt 0.0 1.0 " << std::endl;
	*track_file->get_stream() << "vt 1.0 1.0 " << std::endl;
	*track_file->get_stream() << "vt 1.0 0.0 " << std::endl;
	*track_file->get_stream() << "vn 0.0 1.0 0.0" << std::endl;

	// now flush it all into the file
	for(std::map<u_int8_t, std::vector<std::string>>::iterator it=_textured_segments.begin(); it!=_textured_segments.end() ; ++it) {
		u_int8_t tex_no = it->first;
		std::vector<std::string> faces = it->second;
		*track_file->get_stream() << "# TEXTURE NO: " << std::to_string(tex_no) << std::endl;
		assert (_wwww_chunk != NULL );

		size_t chunk_no  = tex_no / 3;
	
		bitmap *texture = fetch_texture(tex_no);
		//bitmap *pal = fetch_palette(tex_no);
		std::string prefix="ROAD_" + std::to_string(chunk_no) + "_";
		std::string mtl_name=prefix+texture->name();
		*track_file->get_stream() << "usemtl " << mtl_name << std::endl;
		//shpi->to_png(pal, texture, prefix);

		material m(texture, mtl_name);
		std::cout << m.to_mtllib() << std::endl;
		*track_mtl_file->get_stream() << m.to_mtllib() << std::endl;

		//std::vector<bitmap *> bmaps = shpi->get_bitmaps();
		//for(std::vector<bitmap*>::iterator it=bmaps.begin(); it != bmaps.end(); ++it) {
		//	bitmap *b = *it;
		//	std::cout << "FOUND BITMAP :" << b->name() << std::endl;
		//}

		
		for(std::vector<std::string>::iterator face = faces.begin(); face != faces.end(); ++face) {
			std::string face_line = *face;
			*track_file->get_stream() << face_line << std::endl;
		}
	}

	track_file->close();
	track_mtl_file->close();

}

void trkd_chunk::prepare_segments_for_faces(track_segment *segment) 
{

	for(int i=0; i<5; i++) {
		//std::cout << "TRACK SEG: TEX NO: " << std::to_string(segment->get_texture(i)) << std::endl;
		for(int row=0; row<7; row++ ) {
			size_t A0 = segment->Xn(row, i);
			size_t A1 = segment->Xn(row, i+1);
			size_t B1 = segment->Xn(row+1, i+1);
			size_t B0 = segment->Xn(row+1, i);
			//A0-A1-B0
			//*track_file->get_stream() << segment->to_string(A0, A1, B0)  << std::endl;
			//B0 - A1 - B1
			//*track_file->get_stream() << segment->to_string(B0, A1, B1)  << std::endl;
			u_int8_t tex_no = segment->get_texture(i);
			_textured_segments[tex_no].push_back(
				segment->to_string(A0, A1, B0, true)
			);
			_textured_segments[tex_no].push_back(
				segment->to_string(B0, A1, B1, false)
			);
			std::vector<std::string> v=_textured_segments[tex_no];
			std::cout << "VECTOR SIZE: " << v.size() << std::endl;
		}
	}
	for(int i=10; i>=6; i--) {
		size_t j = i - 1;
		if(i==6) j=0; // 6 connects back to 0
		for(int row=0; row<7; row++ ) {
			size_t A0 = segment->Xn(row, i);
			size_t A1 = segment->Xn(row, j);
			size_t B1 = segment->Xn(row+1, j);
			size_t B0 = segment->Xn(row+1, i);
			//A0-A1-B0
			//*track_file->get_stream() << segment->to_string(A0, A1, B0)  << std::endl;
			//B0 - A1 - B1
			//*track_file->get_stream() << segment->to_string(B0, A1, B1)  << std::endl;

			u_int8_t tex_no = segment->get_texture(i-1);
			_textured_segments[tex_no].push_back(
				segment->to_string(A0, A1, B0, true)
			);
			_textured_segments[tex_no].push_back(
				segment->to_string(B0, A1, B1, false)
			);
		}
	}

}


void trkd_chunk::serialize_track_segment_strip_into_verices(std::vector<vertex*> *_X) 
{

	nfs::fstream *track_file;
	track_file = new nfs::fstream(_file->path() + ".obj", std::ios::out|std::ios::app);
	printf("SIZEOF()=%lu\n", _X->size());

	for(std::vector<vertex*>::iterator it = _X->begin(); it != _X->end(); ++it) {
		vertex *v = *it;
		*track_file->get_stream() << v->to_float(0.000180f)  << std::endl;
	}
	track_file->close();
}


#define A(i) segment->_A.at(i)
#define B(i) segment->_B.at(i)
#define C(i) segment->_C.at(i)
#define D(i) segment->_D.at(i)
#define E(i) segment->_E.at(i)

typedef struct { float x0, y0, x1, y1; } texuv;

void trkd_chunk::serialize_track_segment(track_segment *segment) {
	static size_t displacecement = 0;
	
	bitmap *T[10];
	texuv uvmap[10];
	int i;

	// prepare textures for the given segment 
	// and recalculate the begin/end of the textures
	for(i=0; i<10; i++) {
		T[i] = texture_bitmap[segment->get_texture(i)];
		float x0 = T[i]->xp() + 1;
		float y0 = T[i]->yp() + 1;
		float x1 = T[i]->xp() + T[i]->width() - 1;
		float y1 = T[i]->yp() + T[i]->height() - 1;
		
		uvmap[i].x0 = x0 / _final_texture->width();
		uvmap[i].x1 = x1 / _final_texture->width();
		uvmap[i].y0 = y0 / _final_texture->height();
		uvmap[i].y1 = y1 / _final_texture->height();

	}

	std::cout << "TEXTURE 0, i.e. ROAD is: " << T[0]->name() << " " << T[0]->xp() <<  " " << T[0]->yp() << "+" << T[0]->width() << "x" << T[0]->height() << std::endl;
	std::cout << "TEXTURE 0 uvmap " << uvmap[0].x0 << " " << uvmap[0].y0  <<  " ---> " << uvmap[0].x1 << " " << uvmap[0].y1 << std::endl;

	
	nfs::fstream *track_file;
	track_file = new nfs::fstream(_file->path() + ".obj", std::ios::out|std::ios::app);


	// Vertices for the segment - right from the center
	for(i=0; i<5; i++) {
		*track_file->get_stream() << A(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << A(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << B(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << C(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << D(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << E(i+1)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << E(i)->to_float(0.000180f)  << std::endl;
	}

	// Left from the center
	for(int i=10; i>=6; i--) {
		size_t j = i - 1;
		if(i==6) j=0; // 6 connects back to 0
		*track_file->get_stream() << A(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << A(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << B(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << B(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << C(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << C(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(i)->to_float(0.000180f)  << std::endl;

		*track_file->get_stream() << D(i)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << D(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << E(j)->to_float(0.000180f)  << std::endl;
		*track_file->get_stream() << E(i)->to_float(0.000180f)  << std::endl;
	}

	// Texture coordindates for the segment - right from center
	for (i=0; i<5; i++) {

		float d = uvmap[i].x1 - uvmap[i].x0;
		float x0 = uvmap[i].x0;
		float x25 = uvmap[i].x0 + d*0.25;
		float x50 = uvmap[i].x0 + d*0.5;
		float x75 = uvmap[i].x0 + d*0.75;
		float x1 = uvmap[i].x1;

		*track_file->get_stream() << "vt " << x1 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x1 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y1  << std::endl;

		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y1  << std::endl;

		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y1  << std::endl;

		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x0 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x0 << " " << uvmap[i].y1  << std::endl;
	}

	// Texture coordindates for the segment - left from the center
	for (i=9; i>=5; i--) {

		float d = uvmap[i].x1 - uvmap[i].x0;
		float x0 = uvmap[i].x0;
		float x25 = uvmap[i].x0 + d*0.25;
		float x50 = uvmap[i].x0 + d*0.5;
		float x75 = uvmap[i].x0 + d*0.75;
		float x1 = uvmap[i].x1;


		*track_file->get_stream() << "vt " << x1 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x1 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y0  << std::endl;

		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x75 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y0  << std::endl;


		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x50 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y0  << std::endl;


		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y0  << std::endl;
		*track_file->get_stream() << "vt " << x25 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x0 << " " << uvmap[i].y1  << std::endl;
		*track_file->get_stream() << "vt " << x0 << " " << uvmap[i].y0  << std::endl;
	}

	for(i=0;i<40;i++) {
		int i0 = displacecement + 4*i+1;
		int i1 = displacecement + 4*i+2;
		int i2 = displacecement + 4*i+3;
		int i3 = displacecement + 4*i+4;
		*track_file->get_stream() << "f " << 
			i0 << "/" <<  i0  << "/1" << " " <<
			i1 << "/" <<  i1  << "/1" << " " <<
			i3 << "/" <<  i3  << "/1" << " " <<
		std::endl;

		*track_file->get_stream() << "f " <<
			i3 << "/" <<  i3  << "/1" << " " <<
			i2 << "/" <<  i2  << "/1" << " " <<
			i1 << "/" <<  i1  << "/1" << " " <<
		std::endl;
	}

	track_file -> close();
	displacecement += 160;
}

void trkd_chunk::parse() {

#if 1
	skip_bytes(12); // unknown purpose
	skip_bytes(12); // XYZ
	skip_bytes(12); // unknown purpose
	u_int32_t scenery_len = get_uint32();

	std::cout << "Expected scenery len in bytes is: " << scenery_len << std::endl;
	std::cout << "Expected no of chunks is : " << scenery_len/0x554 << std::endl;

	_file->position = 0x1b000;
#endif

	size_t file_sz = _file->get_size();
	size_t trs_no = 0;
	for(;_file->position < file_sz;) {
		printf("PPP: %lu\n", _file->position);
		parse_track_segment(trs_no++);
	}
	//_file->position = save_position + 0x554;
}


//
// Determine all textures needed and create a superbitmap
//
void trkd_chunk::prepare_texture()
{	
	std::vector<bitmap*> __all_textures;
	bitmap *palette = NULL;

	for(std::vector<track_segment*>::iterator it = _segments.begin(); it != _segments.end(); ++it) {
		track_segment *segment = *it;
		for(int i=0; i<10; i++) {
			// fetch all textures now
			u_int8_t tex_no = segment->get_texture(i);

			assert (_wwww_chunk != NULL );

			bitmap *texture = fetch_texture(tex_no);
			texture_bitmap[tex_no]=texture;
			if(palette==NULL) palette = fetch_palette(tex_no);
			//bitmap *pal = fetch_palette(tex_no);
			//palette = pal;
		}
	}

	for(std::map<u_int8_t, bitmap *>::iterator it=texture_bitmap.begin(); it!=texture_bitmap.end() ; ++it) {
		u_int8_t tex_no = it->first;
		bitmap *b = it->second;	
		if(b->is_palette()) continue;
		b->set_png_prefix("ROAD_BITMAP_" + std::to_string(tex_no) + "_");
		bitmap::to_png(palette, b);
		__all_textures.push_back(b);
	}
	_final_texture = bitmap::create_final_bitmap(__all_textures);
	_final_texture->set_png_prefix("ROAD_FINAL_");
	bitmap::to_png(palette, _final_texture);

	nfs::fstream *track_mtl_file;
	track_mtl_file = new nfs::fstream(_file->path() + ".mtl", std::ios::out|std::ios::app);
	material m(_final_texture, "ROAD_FINAL_" + _final_texture->name());
	*track_mtl_file->get_stream() << m.to_mtllib() << std::endl;
	track_mtl_file->close();
}

void trkd_chunk::serialize() {

	nfs::fstream *track_file;
	nfs::fstream *mtl_file;
	track_file = new nfs::fstream(_file->path() + ".obj", std::ios::out);
	mtl_file = new nfs::fstream(_file->path() + ".mtl", std::ios::out);
	*track_file->get_stream() << "mtllib "  << _file->path() << ".mtl" << std::endl;
	track_file->close();
	mtl_file->close();

	prepare_texture();
	// remove a couple of segments from the end in open roads
	if(!closed_road()) {
		_segments.pop_back();
		_segments.pop_back();
		_segments.pop_back();
	}
	else {

		// we always skip the 0th segment
		track_segment *first_segment = _segments.at( 1 ) ;
		track_segment *last_segment = _segments.at( _segments.size() -1 ) ;


		std::vector<vertex*> last_vertex_row = last_segment->_E;
		std::vector<vertex*> first_vertex_row = first_segment->_A;


		last_segment->_E = first_segment->_A;
	}

	bool _first_segment = true;
	for(std::vector<track_segment*>::iterator it = _segments.begin(); it != _segments.end(); ++it) {
		if(_first_segment) {
			_first_segment = false;
			continue;
		}
		track_segment *segment = *it;
		serialize_track_segment(segment);
		//	return;
	}
	//for(std::vector<track_segment*>::iterator it = _segments.begin(); it != _segments.end(); ++it) {
	//	track_segment *segment = *it;
	//	prepare_segments_for_faces(segment);
	//}

	//serialize_track_into_faces();
}

}

}
