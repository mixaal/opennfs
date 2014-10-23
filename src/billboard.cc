#include <opennfs/billboard.h>
#include <opennfs/opengl.h>

namespace visualizer {


void Billboard::sphericalBegin(
			float objPosX, float objPosY, float objPosZ) {

	float lookAt[3],objToCamProj[3], objToCam[3], upAux[3];
	float modelview[16],angleCosine;

	float camX = camera[0];
	float camY = camera[1];
	float camZ = camera[2];

	glPushMatrix();

// objToCamProj is the vector in world coordinates from the 
// local origin to the camera projected in the XZ plane
	objToCamProj[0] = camX - objPosX ;
	objToCamProj[1] = 0;
	objToCamProj[2] = camZ - objPosZ ;

// This is the original lookAt vector for the object 
// in world coordinates
	lookAt[0] = 0;
	lookAt[1] = 0;
	lookAt[2] = 1;


// normalize both vectors to get the cosine directly afterwards
	mathsNormalize(objToCamProj);

// easy fix to determine wether the angle is negative or positive
// for positive angles upAux will be a vector pointing in the 
// positive y direction, otherwise upAux will point downwards
// effectively reversing the rotation.

	mathsCrossProduct(upAux,lookAt,objToCamProj);

// compute the angle
	angleCosine = mathsInnerProduct(lookAt,objToCamProj);

// perform the rotation. The if statement is used for stability reasons
// if the lookAt and objToCamProj vectors are too close together then 
// |angleCosine| could be bigger than 1 due to lack of precision
   if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
      glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[1], upAux[2]);	
      
// so far it is just like the cylindrical billboard. The code for the 
// second rotation comes now
// The second part tilts the object so that it faces the camera

// objToCam is the vector in world coordinates from 
// the local origin to the camera
	objToCam[0] = camX - objPosX;
	objToCam[1] = camY - objPosY;
	objToCam[2] = camZ - objPosZ;

// Normalize to get the cosine afterwards
	mathsNormalize(objToCam);

// Compute the angle between objToCamProj and objToCam, 
//i.e. compute the required angle for the lookup vector

	angleCosine = mathsInnerProduct(objToCamProj,objToCam);


// Tilt the object. The test is done to prevent instability 
// when objToCam and objToCamProj have a very small
// angle between them

	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		if (objToCam[1] < 0)
			glRotatef(acos(angleCosine)*180/3.14,1,0,0);	
		else
			glRotatef(acos(angleCosine)*180/3.14,-1,0,0);	
      
}

void Billboard::cheatSphericalBegin() {
	
	float modelview[16];
	int i,j;

	// save the current modelview matrix
	glPushMatrix();

	// get the current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	// undo all rotations
	// beware all scaling is lost as well 
	for( i=0; i<3; i++ ) 
	    for( j=0; j<3; j++ ) {
		if ( i==j )
		    modelview[i*4+j] = 1.0;
		else
		    modelview[i*4+j] = 0.0;
	    }

	// set the modelview with no rotations
	glLoadMatrixf(modelview);
}
void Billboard::end() {

	// restore the previously 
	// stored modelview matrix
	glPopMatrix();
}


void Billboard::rotate(float *x, float *y, float a)
{
	float xx = *x*cos(a) - *y*sin(a);
	float yy = *x*sin(a) + *y*cos(a);

	*x = xx;
	*y = yy;
}

void Billboard::draw(float x, float y, float z, float width, float height)
{

	x+=_position[0];
	y+=_position[1];
	z+=_position[2];


	float Ax = -width;
	float Ay = -height;
	float Az = 0;

	float Bx = +width;
	float By = -height;
	float Bz = 0;

	float Cx = +width;
	float Cy = +height;
	float Cz = 0;


	float Dx = -width;
	float Dy = +height;
	float Dz = 0;

	float o[3] = {
		x-camera[0], 
		y-camera[1],
		z-camera[2]
	};


#if 1
	float d = sqrt(o[0]*o[0] + o[1]*o[1] + o[2]*o[2]);
	float Oxz = sqrt(o[0]*o[0] + o[2]*o[2]);
	float theta = 0.0f;
	if(Oxz!=0.0f) {
		theta = 3.1415/2 - acosf(o[0] / Oxz);
	}
	


	float phi = asinf(o[1] / d);  // point elevation above camera

	//printf("y=%f cy=%f o[1]=%f d=%f phi=%f\n", y, camera[1], o[1], d, 180.0f*phi/3.141592);
	rotate(&Ay, &Az, phi);
	rotate(&By, &Bz, phi);
	rotate(&Cy, &Cz, phi);
	rotate(&Dy, &Dz, phi);

	rotate(&Ax, &Az, theta);
	rotate(&Bx, &Bz, theta);
	rotate(&Cx, &Cz, theta);
	rotate(&Dx, &Dz, theta);

	if(o[2]>0) {
		Az=-Az;
		Bz=-Bz;
		Cz=-Cz;
		Dz=-Dz;
	}
#endif
#if 0
	printf("R=%f\n", width);
	printf("A [ %f %f %f]\n", Ax, Ay, Az);
	printf("B [ %f %f %f]\n", Bx, By, Bz);
	printf("C [ %f %f %f]\n", Cx, Cy, Cz);
	printf("D [ %f %f %f]\n", Dx, Dy, Dz);
#endif

	x-=_position[0];
	y-=_position[1];
	z-=_position[2];

	Ax+=x;
	Ay+=y;
	Az+=z;

	Bx+=x;
	By+=y;
	Bz+=z;

	Cx+=x;
	Cy+=y;
	Cz+=z;

	Dx+=x;
	Dy+=y;
	Dz+=z;

	//glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(Ax, Ay, Az);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(Bx, By, Bz);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(Cx, Cy, Cz);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(Dx, Dy, Dz);
	//glEnd();

}

}
