#ifndef _OPENNFS_CAMERA_H_
#define _OPENNFS_CAMERA_H_


#include <opennfs/types.h>

namespace visualizer {

class Camera {
	public:
		Camera() {
			_azimuth = 0.0f;
			_elevation = 0.0f;
			_velocity = 0.0f;
			_position.x = _position.y = _position.z = 0.0f;
		}
		
		Camera(float x, float y, float z, float azimuth, float elevation, float velocity) : 
			 _azimuth(azimuth), _elevation(elevation), _velocity(velocity) {
			_position.x = x;
			_position.y = y;
			_position.z = z;
		}
		void set_position(float x, float y, float z);
		void set_position(float position[3]);
		void look_at(float azimuth, float elevation);
		void set_velocity(float velocity);

		float get_velocity();
		float get_azimuth();
		float get_elevation();
		types::XYZ get_position();
		void update();
		types::XYZ get_forward();
		void update_altitude(float dy);
		float get_altitude() ;
		void set_altitude(float y);
		void look_straight();

	private:
		types::XYZ direction_vector();
		types::XYZ _position;
		float _azimuth;
		float _elevation;
		float _velocity;
};

}

#endif /* _OPENNFS_CAMERA_H_ */
