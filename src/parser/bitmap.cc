#include <string>
#include "nfs_io.h"
#include "assert.h"
#include <string.h> // memset
#include "bitmap.h"
#include <stdexcept>
#include "histogram.h"
#include <png.h>

namespace nfs {
	namespace fam {
		void bitmap::parse() {
			_file->position = offset;
			//_file->position += 4; // 0x7b, block size: 3bytes
			u_int32_t code = 0xff & _file->get_uint32();
			_width = _file->get_uint16();
			_height = _file->get_uint16();
			u_int32_t unknown = _file->get_uint32();
			_xp = _file->get_uint16();
			_yp = _file->get_uint16();
			_xp = _yp = 0; //FIXME
			// dirty hack since height is sometimes equal 1 in the palette
			if ( _name == "!PAL" && _width == 256 && _height == 1 ) _height = 3;
			_file->read_bytes(&_data, _width * _height);
			printf("\tFound bitmap: %p %s [%x] %u %u\n", this, _name.c_str(), code, _width,
					_height);

		}

		void bitmap::wrap() {
		}
		
		void bitmap::scale(size_t width, size_t height) 
		{
			if (is_palette()) return;

			char *scaled = new char[width * height];
			float dx = (float)this->width()/width;
			float dy = (float)this->height()/height;

			float src_y = 0;
			for(size_t y=0; y<height; y++, src_y+=dy) {
				float src_x = 0;
				for(size_t x=0 ;x<width; x++, src_x+=dx) {
					size_t xx = (size_t)floor(src_x+0.5f);
					size_t yy = (size_t)floor(src_y+0.5f);
					if(xx>=this->width()) xx=this->width()-1;
					if(yy>=this->height()) yy=this->height()-1;
					//printf("SCALE: %lu %lu [%lu %lu]\n", this->width(), this->height(), xx, yy);
					char d= _data[xx+yy*this->width()];
					scaled[x+y*width] = d;
				}
			}
			_data = scaled;
			_xp=_yp = 0;
			_width = width;
			_height = height;	
		}

		std::string bitmap::serialize() {
			throw std::runtime_error("bitmap::serialize() not implemented");
		}
		bool bitmap::is_palette() {
			//printf("***DBUG: %p w=%u h=%u n=%s\n", this, _width, _height, _name.c_str());
			return _width == 256 && _height == 3 && (_name == "!PAL" || _name == "!pal");
		}
		std::string bitmap::name() { return _name; }
		const u_int32_t bitmap::align(u_int32_t x) const { 
			u_int32_t mask = ~31;
			return (x&31) ? 32 + (x&mask) : x;
		}
		void bitmap::set_width_pow2(int w) {
			_width = next_highest_power_of_2(w);
		}
		void bitmap::set_height_pow2(int h) {
			_height = next_highest_power_of_2(h);
		}
		void bitmap::set_name(std::string name) {
			_name = name;
		}
		void bitmap::equalize(bitmap *palette) {
	nfs::adt::channel_histogram red_channel, green_channel, blue_channel;
	if (is_palette()) {
		return;
	}
	assert (palette->is_palette());
	palette->_eq2();
	return;
	// compute the value distribution
	for (u_int32_t y=0; y<height(); y++) {
		for (u_int32_t x=0; x<width(); x++) {
			unsigned char rgb[3];
			unsigned char _index = getpixel(x, y);
			//if (_index==0xff) { 
				//transparent_pixels++;
			//	continue; // transparency
			//}
		        palette->get_rgb(_index, rgb);
			unsigned char _red = rgb[0];
			unsigned char _green = rgb[1];
			unsigned char _blue = rgb[2];
			red_channel.increase_occurence(_red);
			green_channel.increase_occurence(_green);
			blue_channel.increase_occurence(_blue);
		}
	}

	printf("\nNORMAL HISTOGRAM\n%s\n", blue_channel.to_string().c_str());
	// compute cdf
	red_channel.cdf();
	green_channel.cdf();
	blue_channel.cdf();
	printf("\nCDF HISTOGRAM\n%s\n", blue_channel.cdf_string().c_str());
	// scale cdf
	size_t pixels = width() * height() ;//- transparent_pixels;
	red_channel.equalize(pixels);
	green_channel.equalize(pixels);
	blue_channel.equalize(pixels);
	printf("\nEQ HISTOGRAM\n%s\n", blue_channel.eq_string().c_str());

	// now scan the palette for individual channels and return the equalized value
	for (u_int32_t idx = 0; idx < 256; idx++ ) {
		unsigned char rgb[3];
		palette->get_rgb(idx, rgb);
		size_t red_scaled = red_channel.equalized_value_for(rgb[0]);
		size_t green_scaled = green_channel.equalized_value_for(rgb[1]);
		size_t blue_scaled = blue_channel.equalized_value_for(rgb[2]);
		palette->set_rgb(
			idx,
			(unsigned char)red_scaled, 
			(unsigned char)green_scaled, 
			(unsigned char)blue_scaled
			);
	}

		}
		void bitmap::_eq2() {
			unsigned char r_max=0, g_max=0, b_max=0;
			for(int idx=0; idx<256;idx++) {
				unsigned char r=_data[3*idx];
				unsigned char g=_data[3*idx+1];
				unsigned char b=_data[3*idx+2];
				if(r>r_max)r_max=r;
				if(g>g_max)g_max=g;
				if(b>b_max)b_max=b;
			}
			float rf = 255.0f / r_max;
			float gf = 255.0f / g_max;
			float bf = 255.0f / b_max;
			for(int idx=0; idx<256;idx++) {
				float r = floor(std::min(255.0f, _data[3*idx] * rf));
				float g = floor(std::min(255.0f, _data[3*idx+1] * gf));
				float b = floor(std::min(255.0f, _data[3*idx+2] * bf));
				_data[3*idx]=(unsigned char)r;
				_data[3*idx+1]=(unsigned char)g;
				_data[3*idx+2]=(unsigned char)b;
			}
		}
		const u_int32_t bitmap::x_end() const {return _xp + _width;}
		const u_int32_t bitmap::y_end() const {return _yp + _height;}
		const u_int32_t bitmap::width_pow2() { return next_highest_power_of_2(_width); }
		const u_int32_t bitmap::height_pow2() { return next_highest_power_of_2(_height); }
		const u_int32_t bitmap::width() const { return _width; }
		const u_int32_t bitmap::height() const { return _height; }
		const u_int32_t bitmap::xp() const { return _xp; }
		const u_int32_t bitmap::yp() const { return _yp; }
		void bitmap::set_xp(u_int32_t x) { _xp = x; }
		void bitmap::set_yp(u_int32_t y) { _yp = y; }
		char *bitmap::data() { return _data; }
		bool bitmap::overlaps(std::vector<bitmap *> other_bitmaps) {
			//printf("   OVERLAP: TR TO PLACE %s @ [%u %u]\n", _name.c_str(), _xp, _yp);
			for(std::vector<bitmap*>::iterator it = other_bitmaps.begin(); it != other_bitmaps.end(); ++it) {
				bitmap *o= *it;
				//printf("   OVERLAP: %s [%u %u] cmp %s [%u %u]\n", _name.c_str(), _xp, _yp, o->name().c_str(), o->xp(), o->yp());
				u_int32_t my_xs = _xp;
				u_int32_t my_ys = _yp;
				u_int32_t my_xe = _xp + _width-1;
				u_int32_t my_ye = _yp + _height-1;
				u_int32_t other_xs = o->xp();
				u_int32_t other_ys = o->yp();
				u_int32_t other_xe = o->xp() + o->width()-1;
				u_int32_t other_ye = o->yp() + o->height()-1;

				if (my_xs <= other_xe && my_xe >= other_xs &&
						my_ys <= other_ye && my_ye >= other_ys) {
					//printf("    OVERLAP: with %s\n", o->name().c_str());
					return true;
				}
			}
			//printf(" FINE DOES NOT OVERLAP AT ALL\n");
			return false;
		}
		bool bitmap::_place_within_range(u_int32_t final_width, u_int32_t final_height, std::vector<bitmap*> other_bitmaps) {
			u_int32_t _save_xp = _xp;
			u_int32_t _save_yp = _yp;
			printf("_place_within_range: %u %u @ [%u %u]\n", final_width, final_height, _xp, _yp);
			for(u_int32_t bx = 0; bx < final_width; bx += 32 ){
				_xp = bx;
				if ( _xp + _width > final_width ) break;
				for(u_int32_t by = 0; by < final_height; by += 32 ){
					// try to place bitmap @ [bx, by]
					_yp = by;
					if ( _yp + _height > final_height ) break;
					if(!overlaps(other_bitmaps)) {
						// we don't overlap with others!
						printf("FOUND THE place @ [%u %u]\n", _xp, _yp);
						processed=true;
						return true;
					}

				}
			}
			// cannot place within the range
			_xp = _save_xp;
			_yp = _save_yp;
			processed = false;
			return false;

		}
		void bitmap::_init_with_transparency() {
			_data = new char[_width*_height];
			memset(_data, 0xff, _width*_height);
		}
		void bitmap::get_rgb(unsigned char index, unsigned char rgb[3]) {
			size_t offset = 3*index;
			rgb[0] = _data[offset];
			rgb[1] = _data[offset+1];
			rgb[2] = _data[offset+2];
		}

		void bitmap::set_rgb(unsigned char index, unsigned char r, unsigned char g, unsigned char b) {
			size_t offset = 3*index;
			_data[offset] = r;
			_data[offset+1] = g;
			_data[offset+2] = b;
		}
		unsigned char bitmap::get_intensity(unsigned char index) {
			unsigned char rgb[3];
			get_rgb(index, rgb);
			float I = floor(0.3f*rgb[0] + 0.6f*rgb[1] + 0.1f*rgb[2]);
			if (I>255.0f) I=255.0f;
			return (unsigned char)I;

		}
		char bitmap::getpixel(u_int32_t x, u_int32_t y) {
			return _data[_width*y + x];
		}
		void bitmap::setpixel(u_int32_t x, u_int32_t y, char pixel) {
			//printf("setpixel(): heap_overflow? _width=%u x=%u y=%u\n", _width, x,y);
			assert( x<_width );
			assert( y<_height );
			_data[_width*y + x] = pixel;
		}

		void bitmap::_copy_bitmap(bitmap *b) {
			printf("copy_bitmap(): %s : O=[%u %u] [%u x %u] into [%u %u]\n", b->name().c_str(), b->xp(), b->yp(), b->width(), b->height(), width(), height());
			for(u_int32_t x = 0; x<b->width(); x++) {
				for(u_int32_t y = 0; y<b->height(); y++) {
					setpixel(b->xp() + x, b->yp() + y, b->getpixel(x, y));
				}
			}
		}
		void bitmap::copy_bitmaps(std::vector<bitmap *> bmaps) {
			for(std::vector<bitmap*>::iterator it = bmaps.begin(); it != bmaps.end(); ++it ) {
				bitmap *b = *it;
				_copy_bitmap(b);
			}
		}
		std::string bitmap::to_string() {
			return "usemtl " + _name; // + " " + std::to_string(_width) + "x" + std::to_string(_height); 
		}
		std::string bitmap::png_name()  { return _png_prefix + _name + ".png"; }
		void bitmap::set_png_prefix(std::string prefix) {
			_png_prefix = prefix;
		}

		u_int32_t  bitmap::next_highest_power_of_2(u_int32_t v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v++;
			v += (v == 0);
			return v;
		}

		void bitmap::scale_bitmaps_to_same_dimensions(std::vector<bitmap*> _bitmaps) 
		{
			size_t max_w = 0;
			size_t max_h = 0;

			//Find maximum dimensions
			for(std::vector<bitmap*>::iterator it=_bitmaps.begin(); it!= _bitmaps.end(); ++it) {
				bitmap *b = *it;
				if(b->is_palette()) continue;
				if(b->width()>max_w) max_w = b->width();
				if(b->height()>max_h) max_h = b->height();
			}

			//scale all bitmaps to the same dimension
			for(std::vector<bitmap*>::iterator it=_bitmaps.begin(); it!= _bitmaps.end(); ++it) {
				bitmap *b = *it;
				if(b->is_palette()) continue;
				b->scale(max_w, max_h);
			}
	
		}


		bitmap * bitmap::create_final_bitmap(std::vector<bitmap *> _bitmaps) 
		{
			std::vector<bitmap *> processed;
			u_int32_t final_width=0, final_height=0;
			u_int32_t max_x=0, max_y=0;

			printf("\tCreating final bitmap()\n");

			scale_bitmaps_to_same_dimensions(_bitmaps);
	
			for(std::vector<bitmap*>::iterator it = _bitmaps.begin(); it != _bitmaps.end(); ++it) {
				bitmap *b = *it;
				b->set_xp(0);
				b->set_yp(0);
				b->processed = false;
				if (b->is_palette()) continue;
				printf("\t\t\t+ want to place: %s %d [%u %u %u %u]\n", b->name().c_str(), b->processed, b->xp(), b->yp(), b->width(), b->height());
				if( !b->processed && 0 == processed.size()) {
					// sort out the first largest bitmap
					processed.push_back(b);
					b->processed = true;
					final_width = b->width_pow2();
					final_height = b->height_pow2();
					max_x = b->width();
					max_y = b->height();
					continue;
				}
				printf("xp=%u yp=%u\n", b->xp(), b->yp());
				// iterate over the processed maps and try to determine the best position there
				if(b->_place_within_range(final_width, final_height, processed)) {
					printf("before: max_x=%u\n", max_x);
					if (max_x < b->xp() + b->width()) {
						max_x = b->xp() + b->width();
					}
					printf("after: max_x=%u\n", max_x);
					if (max_y < b->yp() + b->height()) {
						max_y = b->yp() + b->height();
					}
				} 
				else {
					// hmmm, if ended up here it means we can't place this bitmap in the current dimensions
					// and we need to decide what is better: scale horizontally or vertically

					// 1) horizontal option:
					u_int32_t xe1 = b->align(max_x) + b->width();
					u_int32_t ye1 = std::max(b->height(), max_y);
					// 2) vertical option:
					u_int32_t xe2 = std::max(b->width(), max_x);
					u_int32_t ye2 = b->align(max_y) + b->height();

					// compute better option: x/y --> 1
					if (fabs(-1+(float)xe1/ye1) < fabs(-1+(float)xe2/ye2)) {
						// horizontal placement
						b->set_xp( b->align( max_x ) );
						b->set_yp( 0 );
						b->processed = true;
						max_x = xe1;
						final_width = b->align(max_x);
					}
					else {
						// vertical placement
						b->set_yp( b->align( max_y ) );
						b->set_xp( 0 );
						b->processed = true;
						max_y = ye2;
						final_height = b->align(max_y);
					}
				}
				printf("2xp=%u yp=%u\n", b->xp(), b->yp());
				processed.push_back(b);
				printf("TOP: %s %d [%u %u %u %u]\n", b->name().c_str(), b->processed, b->xp(), b->yp(), b->width(), b->height());
				//to_png(palette, b);
			}
			printf("\t\t\tFINAL DIMENSION: [%u %u]\n", final_width, final_height);
			bitmap *final_bitmap = new bitmap("final_bitmap", new null_fstream(), 0);
			final_bitmap->set_width_pow2(final_width);
			final_bitmap->set_height_pow2(final_height);
			final_bitmap->_init_with_transparency();
			final_bitmap->copy_bitmaps(processed);
			return final_bitmap;

		}

		int bitmap::to_png(bitmap *palette, bitmap *texture) 
		{
			static size_t f_no = 0;
			f_no++;
			std::string path = texture->png_name();
			char *tex_data = texture->data();
			char *p_data = palette->data();

			FILE * fp;
			png_structp png_ptr = NULL;
			png_infop info_ptr = NULL;
			size_t x, y;
			png_byte ** row_pointers = NULL;
			/* "status" contains the return value of this function. At first
			   it is set to a value which means 'failure'. When the routine
			   has finished its work, it is set to a value which means
			   'success'. */
			int status = -1;
			/* The following number is set by trial and error only. I cannot
			   see where it it is documented in the libpng manual.
			 */
			int pixel_size = 4;
			int depth = 8;

			fp = fopen(path.c_str(), "wb");
			if (!fp) {
				perror("fopen()");
				goto fopen_failed;
			}

			png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (png_ptr == NULL) {
				fprintf(stderr, "png_create_write_struct_failed!\n");
				goto png_create_write_struct_failed;
			}

			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == NULL) {
				fprintf(stderr, "png_create_info_struct_failed\n");
				goto png_create_info_struct_failed;
			}

			/* Set up error handling. */

			if (setjmp(png_jmpbuf(png_ptr))) {
				goto png_failure;
			}

			/* Set image attributes. */

			png_set_IHDR(png_ptr, info_ptr, texture->width(), texture->height(), depth,
					PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

			/* Initialize rows of PNG. */

			row_pointers = (png_byte**) png_malloc(png_ptr,
					texture->height() * sizeof(png_byte *));
			for (y = 0; y < texture->height(); ++y) {
				png_byte *row = (png_byte *) png_malloc(png_ptr,
						sizeof(u_int8_t) * texture->width() * pixel_size);
				row_pointers[y] = row;
				for (x = 0; x < texture->width(); ++x) {
					u_int8_t pixel = tex_data[x + y * texture->width()];
					u_int8_t red = p_data[3 * pixel];
					u_int8_t green = p_data[3 * pixel + 1];
					u_int8_t blue = p_data[3 * pixel + 2];
					u_int8_t alpha = 0xff;
					if (pixel == 0xff)
						alpha = 0x00;
					*row++ = red;
					*row++ = green;
					*row++ = blue;
					*row++ = alpha;
				}
			}

			/* Write the image data to "fp". */

			png_init_io(png_ptr, fp);
			png_set_rows(png_ptr, info_ptr, row_pointers);
			png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

			/* The routine has successfully written the file, so we set
			   "status" to a value which indicates success. */

			status = 0;

			for (y = 0; y < texture->height(); y++) {
				png_free(png_ptr, row_pointers[y]);
			}
			png_free(png_ptr, row_pointers);

			png_failure: 
			png_create_info_struct_failed: 
			png_destroy_write_struct(
			     &png_ptr, &info_ptr
			);
			png_create_write_struct_failed: 
			fclose(fp);
			fopen_failed: 
			return status;
		}

	}
}
