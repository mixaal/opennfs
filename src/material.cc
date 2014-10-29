/*
 * material.cc
 *
 *  Created on: 3. 11. 2013
 *      Author: mikc
 */

#include <iostream>
#include <opennfs/material.h>
#include <opennfs/opengl.h>
#include <opennfs/sdl.h>
#include <opennfs/gauss.h>
#include <opennfs/field.h>

namespace visualizer {

std::map<std::string, GLuint> Material::textures;


void Material::texture_from_data(size_t width, size_t height, Uint32 *pixels)
{
	float aniso = 0.0f;
	glGenTextures( 1, &texno );
	glBindTexture( GL_TEXTURE_2D, texno );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	// anisotropic filtering
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Material::wyvill_texture(float r, float g, float b, float a, size_t width)
{
	statistics::WyvillFieldFunction field(1.0f);
	float *Gxy = field.sample(1.0f, width);

	Uint32 *pixels = new Uint32[width*width];

	for(size_t i=0; i<width; i++) {
		for(size_t j=0; j<width; j++) {
			float field_Y = Gxy[j + i*width];
			Uint32 R = 255.0f * r * field_Y;
			Uint32 G = 255.0f * g * field_Y;
			Uint32 B = 255.0f * b * field_Y;
			Uint32 A = 255.0f * a * field_Y;
			R=G=B=200.0f;
			//A=255.0f;
	
			//std::cout << R << "-" << G << "-" << B << "-" << A << std::endl;
			pixels[j + i*width] = 
				(A<<24) | (B<<16) | (G<<8) | R ;
		}
	}
#if 0
	for(size_t i=0; i<width; i++) {
		pixels[i] = 0xffffffff;
		pixels[i*width] = 0xffffffff;
	}
#endif

	glGenTextures( 1, &texno );
	glBindTexture( GL_TEXTURE_2D, texno );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, width, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}


void Material::gauss_texture(float r, float g, float b, float a, size_t width)
{
	statistics::Gauss gauss(.4f, .5f);
	float *Gxy = gauss.sample(1.0f, width);

	Uint32 *pixels = new Uint32[width*width];

	for(size_t i=0; i<width; i++) {
		for(size_t j=0; j<width; j++) {
			float gauss_Y = Gxy[j + i*width];
			Uint32 R = 255.0f * r * gauss_Y;
			Uint32 G = 255.0f * g * gauss_Y;
			Uint32 B = 255.0f * b * gauss_Y;
			Uint32 A = 255.0f * a * gauss_Y;
	
			//std::cout << R << "-" << G << "-" << B << "-" << A << std::endl;
			pixels[j + i*width] = 
				(A<<24) | (B<<16) | (G<<8) | R ;
		}
	}

	glGenTextures( 1, &texno );
	glBindTexture( GL_TEXTURE_2D, texno );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, width, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Material::add_texture(std::string texname) {
 	std::cout << "Loading texture:" << texname << std::endl;

	SDL_Surface *tex=IMG_Load(texname.c_str());
#if 0
	int x,y;
	Uint32 *pixels = (Uint32*)tex->pixels;
	for (y=0; y<tex->h; y++) {
	printf("\n");
	for (x=0; x<tex->w; x++) {
		if( y & 1 ) {

			if ( x & 1 ) {
				printf("X");
				*pixels=0xffffffff;
			}
			else {
				printf("*");
				*pixels=0xffffffff;
			}

		}	
		else {
			if ( x & 1 ) {
				printf("*");
				*pixels=0xffffffff;
			}
			else {
				printf("X");
				*pixels=0xffffffff;
			}
		}


		pixels++;
	}
	}
#endif

	if(tex) {
 	      // get the number of channels in the SDL surface
		GLenum texture_format;
		GLint  nOfColors;

		// Check that the image's width is a power of 2
		if ( (tex->w & (tex->w - 1)) != 0 ) {
			printf("warning: image.bmp's width is not a power of 2\n");
		}

		// Also check if the height is a power of 2
		if ( (tex->h & (tex->h - 1)) != 0 ) {
			printf("warning: image.bmp's height is not a power of 2\n");
		}


		nOfColors = tex->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
			if (tex->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		} else if (nOfColors == 3)     // no alpha channel
		{
			if (tex->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			printf("warning: the image is not truecolor..  this will probably break\n");
			exit(-1);
		}

		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texno );

		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texno );
#if 0
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, tex->w, tex->h, 0, texture_format, GL_UNSIGNED_BYTE, tex->pixels );
#else


	printf("%d %d %d\n", texture_format, GL_RGBA, GL_BGR);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gluBuild2DMipmaps(GL_TEXTURE_2D,nOfColors,tex->w,tex->h, texture_format,GL_UNSIGNED_BYTE,tex->pixels);
#endif

		SDL_FreeSurface(tex);
	}

}

void Material::ambient(float r, float g, float b, float a) {
	printf("ambient: [%f %f %f %f]\n", r, g, b, a);
	Ka[0]=r;
	Ka[1]=g;
	Ka[2]=b;
	Ka[3]=a;
}
void Material::diffuse(float r, float g, float b, float a) {
	printf("ambient: [%f %f %f %f]\n", r, g, b, a);
	Kd[0]=r;
	Kd[1]=g;
	Kd[2]=b;
	Kd[3]=a;
}
void Material::specular(float r, float g, float b, float a) {
	printf("ambient: [%f %f %f %f]\n", r, g, b, a);
	Ks[0]=r;
	Ks[1]=g;
	Ks[2]=b;
	Ks[3]=a;
}
void Material::shiness(float Nexp) {
	nexp = Nexp;
}
void Material::draw() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
	glMateriali(GL_FRONT, GL_SHININESS, nexp);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texno);
}
};
