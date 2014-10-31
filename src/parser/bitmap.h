#ifndef _BITMAP_H
#define _BITMAP_H 1
#include <string>
#include "nfs_io.h"
#include "assert.h"
#include <string.h> // memset

namespace nfs {
	namespace fam {
		class bitmap {
			public:
				bitmap(std::string name, fstream *file, size_t offset) : _name(name), _file(file->clone()), offset(offset) { }
				bitmap(std::string name) : _name(name) { }
				bool is_null() { return _is_null == true ; }
				void scale(size_t width, size_t height) ;
				static void scale_bitmaps_to_same_dimensions(std::vector<bitmap*> _bitmaps) ;
				void wrap();
				void parse();
				std::string serialize();
				bool is_palette();
				std::string name();
				const u_int32_t align(u_int32_t x) const;
				void set_width_pow2(int w);
				void set_height_pow2(int h);
				void set_name(std::string name);
				void equalize(bitmap *palette);
				void _eq2() ;
				const u_int32_t x_end() const ;
				const u_int32_t y_end() const ;
				const u_int32_t width_pow2() ;
				const u_int32_t height_pow2();
				const u_int32_t width() const ;
				const u_int32_t height() const ;
				const u_int32_t xp() const ;
				const u_int32_t yp() const ;
				void set_xp(u_int32_t x) ;
				void set_yp(u_int32_t y) ;
				char *data() ;
				bool overlaps(std::vector<bitmap *> other_bitmaps);
				bool _place_within_range(u_int32_t final_width, u_int32_t final_height, std::vector<bitmap*> other_bitmaps);
				void _init_with_transparency();
				void get_rgb(unsigned char index, unsigned char rgb[3]);
				void set_rgb(unsigned char index, unsigned char r, unsigned char g, unsigned char b);
				unsigned char get_intensity(unsigned char index);
				char getpixel(u_int32_t x, u_int32_t y);
				void setpixel(u_int32_t x, u_int32_t y, char pixel);
				void _copy_bitmap(bitmap *b);
				void copy_bitmaps(std::vector<bitmap *> bmaps);
				std::string to_string();
				std::string png_name();
				void set_png_prefix(std::string prefix);
				bool processed=false;
				static bitmap * create_final_bitmap(std::vector<bitmap *> _bitmaps);
				static int to_png(bitmap *palette, bitmap *texture);
				
			private:
				std::string _name;
				std::string _png_prefix="";
				fstream *_file;
				size_t offset;
				u_int32_t  next_highest_power_of_2(u_int32_t v);
			protected:
				u_int32_t _width, _height, _xp, _yp;
				char *_data;
				bool _is_null = false;
		};
		class compare_two_bitmaps:  public std::binary_function<bitmap *, bitmap*, bool>
		{
			public:
				bool operator()(const bitmap *b1, const bitmap *b2) const
				{
					//printf("COMPARATOR OP IN USE: \n");
					//printf("b1=%s [%lu %lu]\n", b1->name().c_str(), b1->width(), b1->height());
					//printf("b2=%s [%lu %lu]\n", b2->name().c_str(), b2->width(), b2->height());
					size_t b1_max = std::max(b1->width(), b1->height());
					size_t b2_max = std::max(b2->width(), b2->height());
					return b1_max > b2_max;
				}
		};

		class null_bitmap : public bitmap {
			public:
				null_bitmap() : bitmap("NULL_Texture", new null_fstream(), 0) {
					_width = _height = 1;
					_data = new char[_width*_height];
					_data[0] = 0x00;
					_is_null = true;
				}
				
		};
	}
}
#endif /* _BITMAP_H */
