#ifndef GAUSS_H_
#define GAUSS_H_

#include <stdlib.h>
#include <math.h>

namespace statistics {


class Gauss {

private:
	float sigma_x, sigma_y;
	float x0 = 0.0f;
	float y0 = 0.0f;
	float sigma2_x_power;
	float sigma2_y_power;

public:
	Gauss(float sigma_x, float sigma_y) :sigma_x(sigma_x), sigma_y(sigma_y) {
		sigma2_x_power = 2.0f * sigma_x * sigma_x;
		sigma2_y_power = 2.0f * sigma_y * sigma_y;
	}
	float Y(float x, float y);
	float *sample(float amplitude, size_t width);
};

};

#endif /* MATRIX_H_ */
