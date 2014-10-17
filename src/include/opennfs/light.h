/*
 * light.h
 *
 *  Created on: 14. 10. 2013
 *      Author: mikc
 */

#ifndef LIGHT_H_
#define LIGHT_H_

namespace visualizer {

class Light {
private:
	int light_no;
	bool _enabled;
	float _ambient[4];
	float _diffuse[4];
	float _specular[4];
	float _position[4];
public:
	Light(int light_no);
	void ambient(float r, float g, float b, float a=1.0f);
	void diffuse(float r, float g, float  b, float a=1.0f);
	void specular(float r, float g, float b, float a=1.0f);
	void position(float x, float y, float z);
	void enable();
	void disable();
	bool isEnabled();
	void draw();

};


};



#endif /* LIGHT_H_ */
