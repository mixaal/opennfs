#include <opennfs/field.h>
#include <stdexcept>

namespace statistics {

float WyvillFieldFunction::Y(float x, float y)
{
	float r = sqrt(x*x + y*y);
	float a = r/R;
	float h = (-4/9) * pow(a, 6) + (17/9) * pow(a, 4) - (22/9) * a * a +1;
	float c = 748*M_PI*R / 405;

	return (r<=R) ? (h/c) : 0.0f;
}


float *WyvillFieldFunction::sample(float amplitude, size_t width) 
{
	if(width==0) 
		throw std::runtime_error("width must not be 0!");
	float *samples = new float[width * width];
	float dy = 2*R / width;
	float dx = 2*R / width;
	
	float y = -R;
	for (size_t i=0; i<width; i++) {
		float x = -R;
		for (size_t j=0; j<width; j++) {
			samples[j + width*i] = amplitude * Y(x, y);
			x+=dx;
		}
		y+=dy;
	}
	return samples;
}
}
