#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <math.h>
namespace visualizer {

#define mathsInnerProduct(v,q) \
	((v)[0] * (q)[0] + \
	(v)[1] * (q)[1] + \
	(v)[2] * (q)[2])


/* a = b x c */

#define mathsCrossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];

inline void mathsNormalize(float *v) {
	float d = (sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2])));
	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;
}



class Billboard {
public:
	Billboard(float camera_position[3]) {
		camera[0] = camera_position[0];
		camera[1] = camera_position[1];
		camera[2] = camera_position[2];
	}


	void sphericalBegin(float x, float y, float z);
	void cheatSphericalBegin();
	void end();

private:
	float camera[3];
	
};
}

#endif /* _BILLBOARD_H_ */
