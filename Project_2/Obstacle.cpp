#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "Obstacle.h"

#define Pi 3.1415926f
#define X 0.525731112119133696
#define Z 0.850650808352039932
//#define random(x) (rand()%x)

/* vertex data array */
static GLfloat vdata[12][3] = {
	{-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
	{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
	{Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

/* triangle indices */
static int tindices[20][3] = {
	{1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
	{1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
	{3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
	{10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};


Obstacle::Obstacle(float positionX, float positionY, float positionZ, float radius){
	this->positionX = positionX;
	this->positionY = positionY;
	this->positionZ = positionZ;
	this->radius = radius;
	init();
}

//float Obstacle::random(int range){
//	float randomNumber;
//	randomNumber = rand()%range *0.01f;
//	return randomNumber;
//}

void Obstacle::init(){
	angle=0.0f;
	animation=true;
	direction = 4;
	speedX = (float)(rand()%5)*0.01-0.025f;
	speedY = (float)(rand()%5)*0.01-0.025f;
	//speedX = 0.05;
	/*speedY = 0.05;*/
	boundaryX = 3.3f, boundaryY = 2.5f;
	flat = 1;            /* 0 = smooth shading, 1 = flat shading */
	subdiv = 0.5;            /* number of subdivisions */
	RGB_R = 1.0f, RGB_G = 0.0f, RGB_B = 0.9f;

}

// Vertices should belong to the unit sphere.
// so we perform normalization
void Obstacle::Normalize(GLfloat v[3]){
	GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (d!=0.0){
		v[0]/=d; 
		v[1]/=d; 
		v[2]/=d;
	}
}

void Obstacle::subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
	float v12[3], v23[3], v31[3];

	if (depth == 0)
	{
		if (flat == 1)
			drawTriangleFlat(v1, v2, v3);
		else
			drawTriangleSmooth(v1, v2, v3);
	}

	/* calculate midpoints of each side */
	for (int i = 0; i < 3; i++)
	{
		v12[i] = (v1[i] + v2[i]) / 2.0f;
		v23[i] = (v2[i] + v3[i]) / 2.0f;
		v31[i] = (v3[i] + v1[i]) / 2.0f;
	}
	// extrude midpoints to lie on unit sphere 
	Normalize(v12);
	Normalize(v23);
	Normalize(v31);

	// recursively subdivide new triangles 
	if (depth != 0)
	{
		subdivide(v1, v12, v31, depth - 1);
		subdivide(v2, v23, v12, depth - 1);
		subdivide(v3, v31, v23, depth - 1);
		subdivide(v12, v23, v31, depth - 1);
	}
}

void Obstacle::drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]){
	glColor3f(RGB_R ,RGB_G ,RGB_B);
	glBegin(GL_TRIANGLES);
		normFace(v1, v2, v3);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();
}

/* draw triangle using sphere normals */
void Obstacle::drawTriangleSmooth(float v1[3], float v2[3], float v3[3])
{
	glColor3f(RGB_R ,RGB_G ,RGB_B);
	glBegin(GL_TRIANGLES);
		glNormal3fv(v1);
		glVertex3fv(v1);
		glNormal3fv(v2);
		glVertex3fv(v2);
		glNormal3fv(v3);
		glVertex3fv(v3);
	glEnd();
}

void Obstacle::normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	GLfloat d1[3];
	GLfloat d2[3];
	GLfloat n[3];
	int k;
	for (k = 0; k < 3; k++)
	{
		d1[k] = v1[k] - v2[k];
		d2[k] = v2[k] - v3[k];
	}
	normCrossProd(d1, d2, n);
	glNormal3fv(n);
}

/* normalized cross product of non-parallel vectors */
void Obstacle::normCrossProd(float u[3], float v[3], float n[3])
{
	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
	Normalize(n);
}

void Obstacle::render(){
	int i;

	/*if(animation){
		angle+=1;
	}
	if(angle>360){
		angle-=360.0;
	}*/
	glPushMatrix();
		glTranslatef(this->positionX, this->positionY, this->positionZ);
		glScaled(this->radius, this->radius, this->radius);
		//glRotatef(angle,1,0,1);
		for (int i = 0; i < 20; i++){
			this->subdivide(vdata[tindices[i][0]],
				vdata[tindices[i][1]],
				vdata[tindices[i][2]],
				subdiv);
		}
	glPopMatrix();
}

void Obstacle::movement(){
	float nextX, nextY;
	if(animation){
		nextX = this->positionX + this->speedX;
		nextY = this->positionY + this->speedY;

		if((nextX+this->radius) > this->boundaryX){
			this->speedX = -this->speedX;
		}
		else if((nextX-this->radius) < -this->boundaryX){
			this->speedX = -this->speedX;
		}

		if((nextY+this->radius) > this->boundaryY){
			this->speedY = -this->speedY;
		}
		else if((nextY-this->radius) < -this->boundaryY){
			this->speedY = -this->speedY;
		}
		this->positionX += this->speedX;
		this->positionY += this->speedY;
	}
}

void Obstacle::stop(){
	this->positionX = 15.0f;
	this->positionY = 15.0f;
	this->animation = false;
}

float Obstacle::random(int range){
	float randomNumber;
	randomNumber = rand()%range *0.01f;
	return randomNumber;
}

void Obstacle::restart(){
	this->positionX = random(660)-3.3f;
	this->positionY = random(500)-2.5f;
	this->animation = true;
}