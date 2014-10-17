/****************************************************************************
 * 			sunsky.cc: a light source using the background
 *      This is part of the yafray package
 *      Copyright (C) 2006  Mathias Wein
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

// Modified by Joseph Miller for texture plugin

// sunsky, from 'A Practical Analytic Model For DayLight" by Preetham, Shirley & Smits.
// http://www.cs.utah.edu/vissim/papers/sunsky/
// based on the actual code by Brian Smits
// and a thread on gamedev.net on skycolor algorithms

#include <math.h>
#include <stdio.h>

#ifndef PFLOAT
typedef float PFLOAT;
#endif

#ifndef CFLOAT
typedef float CFLOAT;
#endif

class vector3d_t
{
	public:
		inline vector3d_t() {}
		inline ~vector3d_t() {}
		vector3d_t& normalize();
		inline void set(PFLOAT sx, PFLOAT sy, PFLOAT sz) { x=sx;y=sy;z=sz; }
		PFLOAT x,y,z;
};

inline vector3d_t& vector3d_t::normalize()
{
	PFLOAT len = x*x + y*y + z*z;
	if (len!=0)
	{
		len = 1.0/sqrt(len);
		x *= len;
		y *= len;
		z *= len;
	}
	return *this;
}

#if 0
class point3d_t
{
	public:
		point3d_t();
		~point3d_t();
};
#endif

class light_t
{
	public:
		inline light_t() {}
		inline ~light_t() {}
};

class renderState_t
{
	public:
		renderState_t();
		~renderState_t();
};

class ray_t
{
public:
	ray_t(): tmin(0), tmax(-1.0), time(0.0) {}
	ray_t(const vector3d_t &d) { dir = d; }
	#if 0
	ray_t(const point3d_t &f, const vector3d_t &d, PFLOAT start=0.0, PFLOAT end=-1.0, PFLOAT ftime=0.0):
		from(f), dir(d), tmin(start), tmax(end), time(ftime) { }

	point3d_t from;
	#endif
	vector3d_t dir;
	mutable PFLOAT tmin, tmax;
	PFLOAT time; //!< relative frame time (values between [0;1]) at which ray was generated
};

class color_t
{
	public:
		friend color_t operator * (const color_t &a, const color_t &b);
		friend color_t operator * (const CFLOAT f,const color_t &b);
		color_t() {R=G=B=0;};
		color_t(CFLOAT r,CFLOAT g,CFLOAT b) {R=r;G=g;B=b;};
		color_t(CFLOAT g) { R=G=B=g; }
		~color_t() {};
		void set(CFLOAT r,CFLOAT g,CFLOAT b) {R=r;G=g;B=b;};

		CFLOAT R,G,B;
};

inline color_t operator * (const color_t &a,const color_t &b)
{
	return color_t(a.R*b.R,a.G*b.G,a.B*b.B);
}

inline color_t operator * (const CFLOAT f,const color_t &b)
{
	return color_t(f*b.R,f*b.G,f*b.B);
}

color_t ComputeAttenuatedSunlight(float theta, int turbidity);

//class sunskyBackground_t: public background_t
class sunskyBackground_t
{
	public:
		//sunskyBackground_t(const point3d_t dir, PFLOAT turb,
		sunskyBackground_t(PFLOAT dir_x, PFLOAT dir_y, PFLOAT dir_z, PFLOAT turb,
			PFLOAT a_var, PFLOAT b_var, PFLOAT c_var, PFLOAT d_var, PFLOAT e_var,
			bool bgl, int bgsamples, CFLOAT pwr);
		virtual color_t operator() (const ray_t &ray, renderState_t &state, bool filtered=false) const;
		virtual color_t eval(const ray_t &ray, bool filtered=false) const;
		//virtual light_t* getLight() const { return envLight; }
		virtual ~sunskyBackground_t();
		#if 0
		//static background_t *factory(paraMap_t &,renderEnvironment_t &);
		static sunskyBackground_t *factory();
		#endif
		color_t getSkyCol(const ray_t &ray) const;
		CFLOAT getPower();
	protected:
		//color_t getSkyCol(const ray_t &ray) const;
		vector3d_t sunDir;
		PFLOAT turbidity;
		double thetaS, phiS;	// sun coords
		double theta2, theta3, T, T2;
		double zenith_Y, zenith_x, zenith_y;
		double perez_Y[5], perez_x[5], perez_y[5];
		double AngleBetween(double thetav, double phiv) const;
		double PerezFunction(const double *lam, double theta, double gamma, double lvz) const;
		light_t *envLight;
		CFLOAT power;
};

CFLOAT sunskyBackground_t::getPower() {
	return power;
}


//sunskyBackground_t::sunskyBackground_t(const point3d_t dir, PFLOAT turb,
sunskyBackground_t::sunskyBackground_t(PFLOAT c_dir_x, PFLOAT c_dir_y, PFLOAT c_dir_z, PFLOAT turb,
		PFLOAT a_var, PFLOAT b_var, PFLOAT c_var, PFLOAT d_var, PFLOAT e_var, bool bgl, int bgsamples, CFLOAT pwr):
		envLight(0), power(pwr)
{
	// default values
	//av = bv = cv = dv = ev = 1.0;	// color variation parameters, default is normal
	//dir.x = dir.y = dir.z = 1;	// direction
	/*PFLOAT dir_x = c_dir_x;
	PFLOAT dir_y = c_dir_y;
	PFLOAT dir_z = c_dir_z;*/
	//turb = 4.0;
	a_var = b_var = c_var = d_var = e_var = 1.0;
//	power = pwr;

	sunDir.set(c_dir_x, c_dir_y, c_dir_z);
	sunDir.normalize();

	thetaS = acos(sunDir.z);
	theta2 = thetaS*thetaS;
	theta3 = theta2*thetaS;
	phiS = atan2(sunDir.y, sunDir.x);
	T = turb;
	T2 = turb*turb;
	double chi = (4.0 / 9.0 - T / 120.0) * (M_PI - 2.0 * thetaS);
	zenith_Y = (4.0453 * T - 4.9710) * tan(chi) - 0.2155 * T + 2.4192;
	zenith_Y *= 1000;  // conversion from kcd/m^2 to cd/m^2

	zenith_x =	( 0.00165*theta3 - 0.00375*theta2 + 0.00209*thetaS)* T2 +
			(-0.02903*theta3 + 0.06377*theta2 - 0.03202*thetaS + 0.00394) * T +
			( 0.11693*theta3 - 0.21196*theta2 + 0.06052*thetaS + 0.25886);

	zenith_y =	( 0.00275*theta3 - 0.00610*theta2 + 0.00317*thetaS)* T2 +
			(-0.04214*theta3 + 0.08970*theta2 - 0.04153*thetaS + 0.00516) * T +
			( 0.15346*theta3 - 0.26756*theta2 + 0.06670*thetaS + 0.26688);

	perez_Y[0] = ( 0.17872 * T - 1.46303) * a_var;
	perez_Y[1] = (-0.35540 * T + 0.42749) * b_var;
	perez_Y[2] = (-0.02266 * T + 5.32505) * c_var;
	perez_Y[3] = ( 0.12064 * T - 2.57705) * d_var;
	perez_Y[4] = (-0.06696 * T + 0.37027) * e_var;

	perez_x[0] = (-0.01925 * T - 0.25922) * a_var;
	perez_x[1] = (-0.06651 * T + 0.00081) * b_var;
	perez_x[2] = (-0.00041 * T + 0.21247) * c_var;
	perez_x[3] = (-0.06409 * T - 0.89887) * d_var;
	perez_x[4] = (-0.00325 * T + 0.04517) * e_var;

	perez_y[0] = (-0.01669 * T - 0.26078) * a_var;
	perez_y[1] = (-0.09495 * T + 0.00921) * b_var;
	perez_y[2] = (-0.00792 * T + 0.21023) * c_var;
	perez_y[3] = (-0.04405 * T - 1.65369) * d_var;
	perez_y[4] = (-0.01092 * T + 0.05291) * e_var;
	
	//if(bgl) envLight = new bgLight_t(this, bgsamples);
};

sunskyBackground_t::~sunskyBackground_t()
{
	if(envLight) delete envLight;
}

double sunskyBackground_t::PerezFunction(const double *lam, double theta, double gamma, double lvz) const
{
  double e1=0, e2=0, e3=0, e4=0;
  if (lam[1]<=230.)
    e1 = exp(lam[1]);
  else
    e1 = 7.7220185e99;
  if ((e2 = lam[3]*thetaS)<=230.)
    e2 = exp(e2);
  else
    e2 = 7.7220185e99;
  if ((e3 = lam[1]/cos(theta))<=230.)
    e3 = exp(e3);
  else
    e3 = 7.7220185e99;
  if ((e4 = lam[3]*gamma)<=230.)
    e4 = exp(e4);
  else
    e4 = 7.7220185e99;
  double den = (1 + lam[0]*e1) * (1 + lam[2]*e2 + lam[4]*cos(thetaS)*cos(thetaS));
  double num = (1 + lam[0]*e3) * (1 + lam[2]*e4 + lam[4]*cos(gamma)*cos(gamma));
  return (lvz * num / den);
}


double sunskyBackground_t::AngleBetween(double thetav, double phiv) const
{
  double cospsi = sin(thetav) * sin(thetaS) * cos(phiS-phiv) + cos(thetav) * cos(thetaS);
  if (cospsi > 1)  return 0;
  if (cospsi < -1) return M_PI;
  return acos(cospsi);
}

inline color_t sunskyBackground_t::getSkyCol(const ray_t &ray) const
{
	vector3d_t Iw = ray.dir;
	Iw.normalize();

	double theta, phi, hfade=1, nfade=1;

	color_t skycolor(0.0);

	theta = acos(Iw.z);
	if (theta>(0.5*M_PI)) {
		// this stretches horizon color below horizon, must be possible to do something better...
		// to compensate, simple fade to black
		hfade = 1.0-(theta*M_1_PI-0.5)*2.0;
		hfade = hfade*hfade*(3.0-2.0*hfade);
		theta = 0.5*M_PI;
	}
	// compensation for nighttime exaggerated blue
	// also simple fade out towards zenith
	if (thetaS>(0.5*M_PI)) {
		if (theta<=0.5*M_PI) {
			nfade = 1.0-(0.5-theta*M_1_PI)*2.0;
			nfade *= 1.0-(thetaS*M_1_PI-0.5)*2.0;
			nfade = nfade*nfade*(3.0-2.0*nfade);
		}
	}

	if ((Iw.y==0.0) && (Iw.x==0.0))
		phi = M_PI*0.5;
	else
		phi = atan2(Iw.y, Iw.x);

	double gamma = AngleBetween(theta, phi);
	// Compute xyY values
	double x = PerezFunction(perez_x, theta, gamma, zenith_x);
	double y = PerezFunction(perez_y, theta, gamma, zenith_y);
	// Luminance scale 1.0/15000.0
	double Y = 6.666666667e-5 * nfade * hfade * PerezFunction(perez_Y, theta, gamma, zenith_Y);

	// conversion to RGB, from gamedev.net thread on skycolor computation
	double X = (x / y) * Y;
	double Z = ((1.0 - x - y) / y) * Y;

	skycolor.set((3.240479 * X - 1.537150 * Y - 0.498535 * Z),
				 (-0.969256 * X + 1.875992 * Y + 0.041556 * Z),
				 ( 0.055648 * X - 0.204043 * Y + 1.057311 * Z));
	//skycolor = power * skycolor;
	return skycolor;
}

color_t sunskyBackground_t::operator() (const ray_t &ray, renderState_t &state, bool filtered) const
{
	return power * getSkyCol(ray);
}

color_t sunskyBackground_t::eval(const ray_t &ray, bool filtered) const
{
	return power * getSkyCol(ray);
}

extern "C" void sunsky_get_rgb(float *red, float *green, float *blue, float sun_x_dir, float sun_y_dir, float sun_z_dir, float turb, float x_coord, float y_coord, float z_coord, float sun_intensity) {
	color_t sky_color;
	vector3d_t vec;
	vec.x = x_coord;
	vec.y = y_coord;
	vec.z = z_coord;
	const ray_t ray(vec);

	//fprintf(stderr, "i = %f\n", sun_intensity);	

	// PFLOAT c_dir_x, PFLOAT c_dir_y, PFLOAT c_dir_z, PFLOAT turb,
	//	PFLOAT a_var, PFLOAT b_var, PFLOAT c_var, PFLOAT d_var, PFLOAT e_var, bool bgl, int bgsamples, CFLOAT pwr
	sunskyBackground_t * new_sunsky = new sunskyBackground_t(sun_x_dir, sun_y_dir, sun_z_dir, turb, 1.0, 1.0, 1.0, 1.0, 1.0, false, 8, sun_intensity);
	sky_color =
		new_sunsky->getSkyCol(
			ray
		);
	//fprintf(stderr, "r = %f", sky_color.R);
	*red = sky_color.R * new_sunsky->getPower();
	//fprintf(stderr, "/%f\n", sky_color.R);
	*green = sky_color.G  * new_sunsky->getPower();
	*blue = sky_color.B * new_sunsky->getPower();

	delete new_sunsky;


}

void hdr(float *r, float *g, float *b, float exposure=1.0f)
{
  *r = 1.0f - exp(-exposure**r);
  *g = 1.0f - exp(-exposure**g);
  *b = 1.0f - exp(-exposure**b);
}

int main(void)
{
float red, green, blue;
float sv_x=1.0f, sv_y=1.0f, sv_z=1.0f;
sunsky_get_rgb(&red, &green, &blue, sv_x, sv_y, sv_z, 4.0f, 1.0f, 1.0f, 1.0f, log10(15.0f));
hdr(&red, &green, &blue);
printf("%f %f %f\n", red, green, blue);
}


#if 0
//static sunskyBackground_t *sunskyBackground_t::factory(paraMap_t &params,renderEnvironment_t &render)
static sunskyBackground_t *sunskyBackground_t::factory(paraMap_t &params,renderEnvironme	const ray_t ray;
nt_t &render)
{
	point3d_t dir(1,1,1);	// same as sunlight, position interpreted as direction
	CFLOAT turb = 4.0;	// turbidity of atmosphere
	bool add_sun = false;	// automatically add real sunlight
	bool bgl = false;
	int bgl_samples = 8;
	double power = 1.0;
	PFLOAT pw = 1.0;	// sunlight power
	PFLOAT av, bv, cv, dv, ev;
	av = bv = cv = dv = ev = 1.0;	// color variation parameters, default is normal

	params.getParam("from", dir);
	params.getParam("turbidity", turb);
	params.getParam("power", power);

	// new color variation parameters
	params.getParam("a_var", av);
	params.getParam("b_var", bv);
	params.getParam("c_var", cv);
	params.getParam("d_var", dv);
	params.getParam("e_var", ev);

	// create sunlight with correct color and position?
	params.getParam("add_sun", add_sun);
	params.getParam("sun_power", pw);
	
	params.getParam("background_light", bgl);
	params.getParam("light_samples", bgl_samples);

	// sunskyBackground_t inherits background_t so I guess background_t classes call each of these members when they need it.
	//background_t * new_sunsky = new sunskyBackground_t(dir, turb, av, bv, cv, dv, ev, bgl, bgl_samples, power);
	sunskyBackground_t * new_sunsky = new sunskyBackground_t(dir, turb, av, bv, cv, dv, ev, bgl, bgl_samples, power);
	
	#if 0
	if (add_sun)
	{
		color_t suncol = ComputeAttenuatedSunlight(acos(fabs(dir.z)), turb);//(*new_sunsky)(vector3d_t(dir.x, dir.y, dir.z));
		double angle = 0.27;
		double cosAngle = cos(angle*M_PI/180.0);
		CFLOAT invpdf = (2.f * M_PI * (1.f - cosAngle));
		suncol *= invpdf * power;

		std::cout << "sunsky: sun color = " << suncol << std::endl;
		paraMap_t p;
		p["type"] = std::string("sunlight");
		p["direction"] = point3d_t(dir[0], dir[1], dir[2]);
		p["color"] = suncol;
		p["angle"] = parameter_t(angle);
		p["power"] = parameter_t(pw);
		
		light_t *light = render.createLight("sunsky_SUN", p);
		if(light)
		{
			(*render.getScene()).addLight(light);
		}
	}
	#endif
	
	return new_sunsky;
}
#endif


