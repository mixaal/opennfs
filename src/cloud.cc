#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <math.h>

#include <opennfs/cloud.h>
#include <opennfs/opengl.h>
#include <opennfs/billboard.h>


namespace visualizer {


float CloudLayer::rayleigh(float a)
{
	return 0.75f * (1 + cos(a)*cos(a));
}

float CloudLayer::henyey_greenstein(float a, float g) 
{
	return (1-g*g)/pow(1+g*g-2*g*cos(a), 1.5f);
}

float CloudLayer::color(float a, float Ik) 
{
	return albedo * extinction * rayleigh(a) * Ik / (4*M_PI);
}


void CloudLayer::position(float x, float y, float z) 
{
	m->position(x, y, z);
	cloud_x = x;
	cloud_y = y;
	cloud_z = z;
}


void CloudLayer::set_observer_position(float x, float y, float z)
{
	eye_x = x;
	eye_y = y;
	eye_z = z;
}

void CloudLayer::set_sun_position(float x, float y, float z)
{
	sun_x = x;
	sun_y = y;
	sun_z = z;
}

void CloudLayer::set_sun_position(float azimuth, float elevation)
{
	sun_azimuth = azimuth;
	sun_elevation = elevation;
}

float CloudLayer::phase_function(types::XYZ p, types::XYZ *delta) {
static int cnt=0;
cnt++;

	p.x+=cloud_x;
	p.y+=cloud_y;
	p.z+=cloud_z;

	float o[3] = {
		p.x-eye_x, 
		p.y-eye_y,
		p.z-eye_z
	};
	float l[3] = {
		sun_x-eye_x,
		sun_y-eye_y,
		sun_z-eye_z
	};

	float Oxz = o[0]*o[0] + o[2]*o[2];
	float Oyz = o[1]*o[1] + o[2]*o[2];
	float os = sqrt(Oxz + o[1]*o[1]);
	float ls = sqrt(l[0]*l[0] + l[1]*l[1] + l[2]*l[2]);

        float Ux = 1.0f;
	float Uy = 0.0f;
	float Uz = 0.0f;

	float Vx = 0.0f;
	float Vy = 1.0f;
	float Vz = 0.0f;
	float cos_a = 1.0f;
	float sin_a = 0.0f;
	if(Oxz >= 0.001f) {
		float Uz2 = o[0]*o[0]/Oxz;
		Uz = sqrt(Uz2);
		Ux = sqrt(1.0f - Uz2);
		if(o[0]>0.0f) Uz=-Uz;
		if(o[2]<0.0f) Uz=-Uz;

		cos_a = o[0] / sqrt(Oxz);
		sin_a = o[2] / sqrt(Oxz);
	}

	float cos_b = sqrt(Oxz) / os; 
	float sin_b = o[1] / os;
	Vx = Ux;
	Vz = Uz;
	Vy = 1.0f; //cos_b;
	//printf("[%f %f %f] : cos_b=%f sin_b=%f cos_a=%f, sin_a=%f, Vx=%f Vz=%f\n", o[0], o[1], o[2], cos_b, sin_b, cos_a, sin_a, Vx, Vy);


	o[0]/=os;
	o[1]/=os;
	o[2]/=os;
	l[0]/=ls;
	l[1]/=ls;
	l[2]/=ls;

	delta->x = Vx;
	delta->y = Vy;
	delta->z = Vz;

	float cos_Phi = (o[0]*l[0] + o[1]*l[1] + o[2]*l[2]);
	float phi = acos(cos_Phi);
#if 0
	if(0==(cnt&511)) {
		std::cout << "P: " << p.x << ", " << p.y << ", " << p.z << std::endl;
		std::cout << "L: " << sun_x << ", " << sun_y << ", " << sun_z << std::endl;
		std::cout << "O: " << eye_x << ", " << eye_y << ", " << eye_z << std::endl;
		std::cout << "O-P: " << o[0] << ", " << o[1] << ", " << o[2] << std::endl;
		std::cout << "P-L: " << l[0] << ", " << l[1] << ", " << l[2] << std::endl;
		std::cout << "COS_PHI: " << cos_Phi << std::endl;
		std::cout << "PHI: " << phi << std::endl;
	}
#endif



	// the phase function should just say the probability of scattering in certain direction
	// however we don't compute that yet.
	// we use the function bellow for glowing the angles close to sun directions and decaling the other
	// ones. That's obviously not scattering and this method is not doing what phase function should do.
	float k = henyey_greenstein(0.99f, phi);
	float sol_elevation_ratio = fabs(2 * sun_elevation / M_PI);
	k=std::max(k, 0.7f + 0.2f*sol_elevation_ratio);
	return k;
}


void CloudLayer::drawVoxel(types::voxel_t *v) 
{

	float red = sun_red * v->c;
	float green = sun_green * v->c;
	float blue = sun_blue * v->c;
	float alpha = blend_factor_from_vapor(v->vapor, dissipation);
	//alpha = 0.01f;
	types::XYZ p = v->p;
#ifdef  _CLOUD_PHASE_FUNCTION 
	/* shade the clouds based on phase function */
	float k = phase_function(p, &(v->delta));
	red *= k;
	green *= k;
	blue *= k;
	red = std::min(1.0f, red);
	green=std::min(1.0f, green);
	blue=std::min(1.0f, blue);
#endif
	glColor4f( red, green, blue, alpha);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//billboard.billboardSphericalBegin(p.x, p.y, p.z);

	float dx = v->delta.x;
	float dy = v->delta.y;
	float dz = v->delta.z;
	//dx=dy=1.0f;
	//dz=0.0f;
	

#if 1
	float r = v->r ;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(p.x-r*dx, p.y-r*dy, p.z-r*dz);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(p.x+r*dx, p.y-r*dy, p.z+r*dz);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(p.x+r*dx, p.y+r*dy, p.z+r*dz);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(p.x-r*dx, p.y+r*dy, p.z-r*dz);
	glEnd();
	//billboard.billboardEnd();
#else
	//dx = 1500.0f / os;
	float r = v->r * dx;
	glPointSize(r);
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
#endif

}

void CloudLayer::draw(float position[3]) {
	//std::cout << "VOXELS SZ: " << voxels.size() << std::endl;
	Billboard billboard(position);

	sort_voxels cmp_voxels(this);
	glPushMatrix();
	glMultMatrixf(m->get());
	    glPointSize(3.0f);
	    glEnable(GL_POINT_SMOOTH) ;
            glBegin(GL_POINTS);
	    glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(sun_x-cloud_x, sun_y-cloud_y, sun_z-cloud_z);
            glEnd();


	std::sort(voxels.begin(), voxels.end(), cmp_voxels);
	std::sort(basement.begin(), basement.end(), cmp_voxels);

	material->draw();

	float c=1.0f;
	for(size_t idx=0; idx<voxels.size(); idx++) {
	//for(size_t idx=0; idx<500; idx++) {
		types::voxel_t *v = voxels.at(idx);
		//billboard.cheatSphericalBegin();
		drawVoxel(v);
		//billboard.end();
	}
#if 1
	base->draw();
	for(size_t idx=0; idx<basement.size(); idx++) {
		types::voxel_t *v = basement.at(idx);
		//billboard.cheatSphericalBegin();
		drawVoxel(v);
		//billboard.end();
	}
#endif

	//exit(0);
	glPopMatrix();

}

void CloudLayer::set_dissipation(float a) {
	dissipation = a;
}

void CloudLayer::read_cloud_map(std::string pathname) {
	std::fstream cloud_map_file(pathname, std::ios::in);

	if( !cloud_map_file.is_open() ) {
		std::cerr << "Error opening file: " << pathname << std::endl;
		throw std::runtime_error("can't open file!");
	}
	std::string rgb_component;
	while (cloud_map_file >> rgb_component) {
		std::stringstream ss;
		ss << std::hex << rgb_component;
		unsigned int rgb;
		ss >> rgb;
		unsigned char mass = rgb&0x0ff;
		cloud_mass.push_back(mass);
	}
	cloud_map_file.close();

	size_t mass_no = cloud_mass.size();
	size_t w = sqrt(mass_no) ;

	std::cout << "SIZE: " << mass_no << " SQRT= " << w << std::endl;
	if(w*w != mass_no) {
		throw std::runtime_error("cloud definition file must be power of two!");
	}
	// initialize cloud mass storage
	cloud_map_width = w;
	cloud3d = new types::voxel_t**[w];
	for(size_t i=0; i<w; i++) {
		cloud3d[i] = new types::voxel_t*[w];
	}
	for(size_t i=0; i<w; i++)
		for(size_t j=0; j<w; j++) {
			cloud3d[i][j] = new types::voxel_t[CLOUD_MAX_LAYER_HEIGHT];
		}
}


void CloudLayer::generate() 
{


	float cx=1.0;
	float cy=0.0;
	float cz=0.0;
	double dx=2.0/128;
	double dy=4.0/128;
	double ra;
	double h;
	int k,t;
	
   types::XYZ p = { cx, cy, cz };

   int cl_cnt=0;
   
	for(size_t j=0;j<cloud_map_width;j++) {
		cx=0.0;
		for(size_t i=0;i<cloud_map_width;i++) {

			ra=0.03;
			p.x=cx * scale;
			p.y=cy * scale;
			p.z=cz * scale;


			k=0;
			h = get_mass(i,j);
			cloud3d[i][j][k].p=p;
			cloud3d[i][j][k].r=10.95;
			cloud3d[i][j][k].h=h;
			cloud3d[i][j][0].top=0;
			cloud3d[i][j][k].draw=0;
			cloud3d[i][j][k].vapor=0;


			//fprintf(stderr,"[%d %d %d]: h=%4f t=%d\n",i,j,k,h,t);
			
			t= (int)(h*h*10);
			//t=(int)(h*10);
			for( cy=0,k=0; k < t ; k++, cy+=dx ) {
				struct {
					float x,y,z;
				} random_shift = {
					-0.5f + random()/(float)(RAND_MAX),
					-0.5f + random()/(float)(RAND_MAX),
					-0.5f + random()/(float)(RAND_MAX)
				};
				p.x=cx * scale;
				p.y=cy * scale;
				p.z=cz * scale;
#ifndef _CLOUD_PARTICLE_RADIUS
#	warning "Please define the _CLOUD_PARTICLE_RADIUS in Makefile."
#       define _CLOUD_PARTICLE_RADIUS 2.5f
#endif
				//double r=0.06f * scale;
				double r = _CLOUD_PARTICLE_RADIUS*scale * 2.0/256 ;
				if(k==0) r*=2;
				p.x += 0.5f * r * random_shift.x;
				p.y += 0.5f * r * random_shift.y;
				p.z += 0.5f * r * random_shift.z;

				cloud3d[i][j][k].p=p;
				cloud3d[i][j][k].r=r;
				cloud3d[i][j][k].h=h;
				cloud3d[i][j][0].top=t;
				cloud3d[i][j][k].draw=1;
				cloud3d[i][j][k].vapor= t-k;
				if( cloud3d[i][j][k].vapor > max_vapor ) max_vapor=t-k;
				
//				fprintf(stderr,"[%d %d %d]: h=%4f t=%d\n",i,j,k,h,t);
				

			}
			cl_cnt+=t;
			cx+=dx;
		}
		cz+=dy;
	}
}

void CloudLayer::optimize() 
{
int cl_removed=0;
types::XYZ p;
	for(size_t i=0;i<cloud_map_width;i++) {
		for(size_t j=0;j<cloud_map_width;j++) {

			for(int k=cloud3d[i][j][0].top;k>=0;k--){


				if( !cloud3d[i][j][k].draw ) continue;
				cloud3d[i][j][k].surf=CLOUD_SURF_NORMAL;

				cloud3d[i][j][k].c=1.0;
			   	cloud3d[i][j][k].a=cloud3d[i][j][k].vapor;
				//std::cout << "cloud H: " << cloud3d[i][j][k].h << "TOP: " << cloud3d[i][j][k].top << std::endl;
				if(k==1) {
					cloud3d[i][j][k].c=1.0-cloud3d[i][j][k].h/3;
				}
				if( !k ) {
					cloud3d[i][j][k].c=1.0-cloud3d[i][j][k].h/2;
				}

#if 0
				if( remove_cloud_cell(i-1,j,k) && remove_cloud_cell(i+1,j,k) && remove_cloud_cell(i,j-1,k) && remove_cloud_cell(i,j+1,k) && remove_cloud_cell(i,j,k-1) && remove_cloud_cell(i,j,k+1) ) {
					cloud3d[i][j][k].draw=0;
					cl_removed++;
				}
#endif

				if( !cloud3d[i][j][k].draw ) continue;


				if( !k) {
					cloud3d[i][j][k].surf=CLOUD_SURF_BOTTOM_LAYER;
				}

				cloud3d[i][j][k].r+=(rand()&0xff)/(double)0xff/50;
				if(k==0) {
					basement.push_back(&cloud3d[i][j][k]);
				}
				else {
					voxels.push_back(&cloud3d[i][j][k]);
				}
				
			}
		}	
	}


}

float CloudLayer::get_mass(size_t i, size_t j)
{
	return cloud_mass.at(i + j*cloud_map_width) / 256.0f;
}


bool CloudLayer::remove_cloud_cell(ssize_t i, ssize_t j, ssize_t k)
{
	if( i<0 ) return false;
	if( i>=cloud_map_width-1 ) return false;
	if( j<0 ) return false;
	if( j>=cloud_map_width-1 ) return false;
	if( k<0 ) return false;
	if( k>=cloud3d[i][j][0].top ) return false;

	if( cloud3d[i][j][0].top <= k) return false;

	return true;

}

float CloudLayer::blend_factor_from_vapor(float vapor, float a)
{
	float v = vapor/max_vapor;
//	return 3*v*v*a;
	return a*vapor*vapor;
}

void CloudLayer::set_sun_color(float color[3])
{
	sun_red = color[0];
	sun_green = color[1];
	sun_blue = color[2];
}

void CloudLayer::evolve(int hour, int minute)
{
	float min_dissipation = 0.0001f;
	float max_dissipation = 1.0f;

	float d = max_dissipation - min_dissipation;

	int start_hour = 8;
	int peak_hour = 14;
	int end_hour = 19;
	int ps = peak_hour - start_hour;
	int pe = end_hour - peak_hour;

	if(hour<start_hour || hour>end_hour)
		dissipation = min_dissipation;

	if(hour<peak_hour) {
		float t = (hour-start_hour) + minute/60.0f;
		dissipation = min_dissipation + t*d/ps;
	}
	else {
		float t = (hour-peak_hour) + minute/60.0f;
		dissipation = max_dissipation - t*d/pe;
	}

}

}
