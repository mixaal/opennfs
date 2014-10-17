#include <opennfs/matrix.h>

namespace visualizer {

void Matrix::position(float x, float y, float z)
{
	m[12] = x;
	m[13] = y;
	m[14] = z;

}

void Matrix::rotate(float x, float y, float z, float angle)
{
	float c=cos(angle*M_PI/180.0f);
	float s=sin(angle*M_PI/180.0f);
	float xy=x*y;
	float xz=x*z;
	float yz=y*z;
	float zs=z*s;
	float xs=x*s;
	float ys=y*s;

	m[0]=x*x*(1-c)+c;
	m[1]=xy*(1-c)+zs;
	m[2]=xz*(1-c)-ys;

	m[4]=xy*(1-c)-zs;
	m[5]=y*y*(1-c)+c;
	m[6]=yz*(1-c)+xs;

	m[8]=xz*(1-c)+ys;
	m[9]=yz*(1-c)-xs;
	m[10]=z*z*(1-c)+c;

}

float * Matrix::get() {
	return m;
}

}
