/*
 * model.h
 *
 *  Created on: 12. 10. 2013
 *      Author: mikc
 */

#ifndef MATRIX_H_
#define MATRIX_H_


#include <string>
#include <string.h>
#include <vector>
#include <math.h>

namespace visualizer {


class Matrix {

private:
	float m[16];

public:
	Matrix() {
		memset(m, 0, sizeof(float)*16);
		m[0] = m[5] = m[10] = m[15] = 1.0f;
	}
	void position(float x, float y, float z);
	void rotate(float x, float y, float z, float angle);
	float *get();
};

};

#endif /* MATRIX_H_ */
