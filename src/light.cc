/*
 * light.cc
 *
 *  Created on: 14. 10. 2013
 *      Author: mikc
 */

#include <opennfs/light.h>
#include <opennfs/opengl.h>

namespace visualizer {

Light::Light(int light_no) : light_no(light_no)
{
	_enabled=false;
};
void Light::ambient(float r, float g, float b, float a)
{
	_ambient[0]=r;
	_ambient[1]=g;
	_ambient[2]=b;
	_ambient[3]=a;
}
void Light::diffuse(float r, float g, float  b, float a)
{
	_diffuse[0]=r;
	_diffuse[1]=g;
	_diffuse[2]=b;
	_diffuse[3]=a;
}
void Light::specular(float r, float g, float b, float a)
{
	_specular[0]=r;
	_specular[1]=g;
	_specular[2]=b;
	_specular[3]=a;
}
void Light::position(float x, float y, float z)
{
	_position[0]=x;
	_position[1]=y;
	_position[2]=z;
	_position[3]=1.0f;
}
void Light::enable()
{
	_enabled=true;
}
void Light::disable()
{
	_enabled=false;
}
bool Light::isEnabled()
{
	return _enabled;
}
void Light::draw() {
	if(isEnabled()) {
		glEnable(light_no);
		glLightfv(GL_LIGHT0, GL_AMBIENT, _ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, _diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, _specular);
		glLightfv(GL_LIGHT0, GL_POSITION, _position);
	}
	else {
		glDisable(light_no);
	}
}

};



