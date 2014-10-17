#ifndef _SKYDOME_H_
#define _SKYDOME_H_

#include <math.h>
#include <iostream>
#include <opennfs/ephem.h>
namespace visualizer {



class SkyDome {
public:
	SkyDome(float latitude, float longitude, int year, int month, int day, int hour, int minute, int sec, float exposure=1.0f, float turbidity=4.0f, float sol_intensity=15.0f, float distance=800.0f):
		latitude(latitude), longitude(longitude), exposure(exposure), turbidity(turbidity), sol_intensity(log10f(sol_intensity)), distance(distance), year(year), month(month), day(day), hour(hour), minute(minute), sec(sec) {
		ephem = new astronomy::Ephemerides(latitude, longitude, year, month, day, hour, minute, sec);
		get_sun_position(&sol_x, &sol_y, &sol_z);
		night_r = 0.08f;
		night_g = 0.05f;
		night_b = 0.15f;
	}
	void draw();
        void increase_turbidity() {
		turbidity += 0.01f;
		std::cout << turbidity << std::endl;
        }
	void draw_stars();
	void update(int year, int month, int day, int hour, int minute, int sec);
	float solX() { return sol_x; }
	float solY() { return sol_y; }
	float solZ() { return sol_z; }

	float sol_elevation() { return sun_elevation; }
	float sol_azimuth() { return sun_azimuth; }

	float hazeRed() { return haze_r; }
	float hazeGreen() { return haze_g; }
	float hazeBlue() { return haze_b; }
private:
	bool close_to_sun(float u, float v);
	void get_sun_position(float *x_vec, float *y_vec, float *z_vec);
	void get_sky_color(float *red, float *green, float *blue, float sun_x_dir, float sun_y_dir, float sun_z_dir, float turb, float x_coord, float y_coord, float z_coord, float sun_intensity);
	void hdr(float *r, float *g, float *b);
	// geographical coordinates
	float latitude, longitude;
	// exposure for hdr rendering
	float exposure;
	// turbidity 3-5 normal day, up to 16
	float turbidity;
	// solar density
	float sol_intensity;
	// sky dome diameter
	float distance;
	// night color
	float night_r, night_g, night_b;
	// haze color
	float haze_r, haze_g, haze_b;
	//date
	int year, month, day;
	//time
	int hour, minute, sec;
	// ephemerides
	astronomy::Ephemerides *ephem;
	// sun vector
	float sol_x, sol_y, sol_z;
	// sun azimuth and elevation above horizon
	float sun_azimuth, sun_elevation;
};


}



#endif /* _SKYDOME_H_ */
