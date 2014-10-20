#include <opennfs/camera.h>
#include <math.h>

namespace visualizer {

void Camera::set_position(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
}


void Camera::set_position(float position[3])
{
	_position.x = position[0];
	_position.y = position[1];
	_position.z = position[2];
}

void Camera::look_at(float azimuth, float elevation)
{
	_azimuth = azimuth;
	_elevation = elevation;
}

void Camera::set_velocity(float velocity)
{
	_velocity = velocity;
}

float Camera::get_velocity() 
{
	return _velocity;
}

float Camera::get_elevation() 
{
	return _elevation;
}

float Camera::get_azimuth()
{
	return _azimuth;
}

types::XYZ Camera::get_position()
{
	return _position;
}

void Camera::look_straight()
{
	_elevation = 0.0f;
}

void Camera::set_altitude(float y)
{
	_position.y = y;
}

float Camera::get_altitude() 
{
	return _position.y;
}

void Camera::update_altitude(float dy)
{
	_position.y += dy;
}

types::XYZ Camera::direction_vector()
{
	float a = M_PI * _azimuth / 180.0f;
	float b = M_PI * _elevation / 180.0f;

	types::XYZ v;

	v.x = cos(a)*cos(b);
	v.y = sin(b);
	v.z = sin(a)*cos(b);

	return v;
}

void Camera::update()
{
	types::XYZ _direction = direction_vector();

	_position.x += _velocity*_direction.x;
	_position.y += _velocity*_direction.y;
	_position.z += _velocity*_direction.z;

}

types::XYZ Camera::get_forward()
{
	types::XYZ target = _position;
	types::XYZ _direction = direction_vector();

	target.x += _direction.x;
	target.y += _direction.y;
	target.z += _direction.z;

	return target;
}

}
