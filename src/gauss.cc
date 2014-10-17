#include <opennfs/gauss.h>
#include <stdexcept>

namespace statistics {

float Gauss::Y(float x, float y)
{
	float dx = x-x0;
	float dy = y-y0;
	
	float dx2 = dx * dx;
	float dy2 = dy * dy;

	return exp(-dx2/sigma2_x_power - dy2/sigma2_y_power);
}


float *Gauss::sample(float amplitude, size_t width) 
{
	if(width==0) 
		throw std::runtime_error("width must not be 0!");
	float *samples = new float[width * width];
	float sigma_x3 = 3*sigma_x;
	float sigma_y3 = 2*sigma_y;
	float dy = 2*sigma_y3 / width;
	float dx = 2*sigma_x3 / width;
	
	float y = -sigma_y3;
	for (size_t i=0; i<width; i++) {
		float x = -sigma_x3;
		for (size_t j=0; j<width; j++) {
			samples[j + width*i] = amplitude * Y(x, y);
			x+=dx;
		}
		y+=dy;
	}
	return samples;
}
}
