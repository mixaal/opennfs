#ifndef FIELD_FUNC_H_
#define FIELD_FUNC_H_

#include <stdlib.h>
#include <math.h>

namespace statistics {


class WyvillFieldFunction {

private:
	float R;

public:
	WyvillFieldFunction(float R) : R(R){
	}
	float Y(float x, float y);
	float *sample(float amplitude, size_t width);
};

};

#endif /* FIELD_FUNC_H_ */
