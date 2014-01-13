#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#include "Character.h"
#define Pi 3.1415926f
#define X 0.525731112119133696
#define Z 0.850650808352039932

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


Character::Character(float positionX, float positionY, float positionZ, float radius, bool AI){
	this->positionX = positionX;
	this->positionY = positionY;
	this->positionZ = positionZ;
	this->originalZPosition = positionZ;
	this->radius = radius;
	this->AI = AI;
	init();
}

void Character::setColor(float R, float G, float B){
	RGB_R = R;
	RGB_G = G;
	RGB_B = B;
}

void Character::setLineColor(float R, float G, float B){
	Line_R = R;
	Line_G = G;
	Line_B = B;
}

void Character::enablePredator(){
	this->predator = true;
}

void Character::disablePredator(){
	this->predator = false;
}

void Character::setRival(Character *rival){
	this->rival = rival;
}

void Character::setPosition(float px, float py){
	this->positionX = px;
	this->positionY = py;
}

void Character::setDirection(int dirc){
	this->direction = dirc;
}

void Character::init(){
	angle=0.0f;
	animation=true;
	predator = true;
	direction = 4;
	speedX = 0.05f;
	speedY = 0.05f;
	boundaryX = 3.3f, boundaryY = 2.5f;
	flat = 0;            /* 0 = smooth shading, 1 = flat shading */
	subdiv = 2;            /* number of subdivisions */
	//RGB_R = 0.5f, RGB_G = 0.0f, RGB_B = 0.0f;
	RGB_R = 1.0f, RGB_G = 1.0f, RGB_B = 1.0f;
	
	if(this->predator){Line_R = 1.0f, Line_G = 0.0f, Line_B = 0.0f;}
	else Line_R = 0.0f, Line_G = 1.0f, Line_B = 0.0f;

	bounceTime = 1.0f;
	jumpAngle = 0;
	jumpState = false;
	rotationAngle = 1.0f;
}

// Vertices should belong to the unit sphere.
// so we perform normalization
void Character::Normalize(GLfloat v[3]){
	GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (d!=0.0){
		v[0]/=d; 
		v[1]/=d; 
		v[2]/=d;
	}
}

void Character::subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
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

void Character::drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]){
	glColor3f(RGB_R ,RGB_G ,RGB_B);
	glBegin(GL_TRIANGLES);
		normFace(v1, v2, v3);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();

	glColor3f(Line_R, Line_G, Line_B);
	glLineWidth(0.5);
	glBegin(GL_LINE_LOOP);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();
}

/* draw triangle using sphere normals */
void Character::drawTriangleSmooth(float v1[3], float v2[3], float v3[3])
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

	glColor3f(Line_R, Line_G, Line_B);
	glLineWidth(0.5);
	glBegin(GL_LINE_LOOP);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
	glEnd();
}

void Character::normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
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
void Character::normCrossProd(float u[3], float v[3], float n[3])
{
	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
	Normalize(n);
}

void Character::setRotationAngle(float angle){
	this->rotationAngle = angle;
}


void Character::render(){
	int i;

	if(animation){
		angle+=rotationAngle;
	}
	if(angle>360){
		angle-=360.0;
	}
	glPushMatrix();
		glTranslatef(this->positionX, this->positionY, this->positionZ);
		glScaled(this->radius, this->radius, this->radius);
		glRotatef(angle,1,0,1);
		for (int i = 0; i < 20; i++){
			this->subdivide(vdata[tindices[i][0]],
				vdata[tindices[i][1]],
				vdata[tindices[i][2]],
				subdiv);
		}
	glPopMatrix();
}

void Character::Jump(){
	jumpState = true;
}

void Character::jumpMovement(){
	if(jumpState){
		jumpAngle += 5;
		if(jumpAngle >= 360){
			jumpAngle = 0;
			bounceTime = bounceTime - 0.4f;
		}
		if(bounceTime < 0){
			jumpState = false;
			bounceTime = 1.0f;
		}
	}
	
	if(jumpState==false){
		this->positionZ = originalZPosition;
	}
	else{
		bounce = sin(Pi/180*jumpAngle);
		/*printf("jumpAngle is %d, and bounce is %f \n",jumpAngle, bounce);
		printf("boucne time is %f", bounceTime);*/
		this->positionZ = originalZPosition + bounceTime * bounce;
	}


}

void Character::collision(){
	this->direction = 4;
	if(!this->predator){
		if(this->radius >= 0.05f){
			this->radius -= 0.01f;
		}
		this->speedX +=0.015f;
		this->speedY +=0.015f;
	}
}

void Character::OriginalState(){
	this->direction = 4;
	this->radius = 0.25;
	this->speedX = 0.05f;
	this->speedY = 0.05f;
	this->rotationAngle = 1.0f;
}

void Character::keyboardProcess(int key, int x, int y){
	switch (key){
	case GLUT_KEY_UP:
		this->direction = 0;
		break;
	case GLUT_KEY_DOWN:
		this->direction = 1;
		break;
	case GLUT_KEY_LEFT:
		this->direction = 2;
		break;
	case GLUT_KEY_RIGHT:
		this->direction = 3;
		break;
	case 32:
		this->Jump();
		break;

	}
}

bool Character::boundaryCheck(float npx, float npy){
	if((npx >= (this->radius-boundaryX)) && (npx <= (boundaryX-this->radius)) && (npy >= (this->radius-boundaryY)) && (npy <= (boundaryY-this->radius))){
		return true;
	}
	else{
		return false;
	}
}

//get the x axis speed by next direction
float Character::getSpeedX(int dire){
	float sX;
	switch(dire){
		case 0:
			sX = 0;
			break;
		case 1:
			sX = 0;
			break;
		case 2:
			sX = -this->speedX;
			break;
		case 3:
			sX = this->speedX;
			break;
	}
	return sX;
}

//get the y axis speed by next direction
float Character::getSpeedY(int dire){
	float sY;
	switch(dire){
		case 0:
			sY = this->speedY;
			break;
		case 1:
			sY = -this->speedY;
			break;
		case 2:
			sY = 0;
			break;
		case 3:
			sY =0;
			break;
	}
	return sY;
}


//get the distance between predator and prey
float Character::getRivalDistance(float npx, float npy){
	float distance;
	distance = sqrt(pow((npx-rival->positionX),2) + pow((npy-rival->positionY), 2));
	return distance;
}

int Character::predatorAI(){
	float rivalPositionX = rival->positionX;
	float rivalPositionY = rival->positionY;
	int nextDirection;
	nextDirection = directionPridictionPredator(rivalPositionX, rivalPositionY);
	return nextDirection;
}

int Character::directionPridictionPredator(float rpx, float rpy){
	int nextDirection = 4;
	float minDistance = 10;
	float nextDistance;
	float nextPosisionX;
	float nextPosisionY;

	for(int i=0; i<4; i++){
		nextPosisionX = (this->positionX + getSpeedX(i));
		nextPosisionY = (this->positionY + getSpeedY(i));
		nextDistance = getRivalDistance(nextPosisionX, nextPosisionY);

		if(nextDistance <= minDistance){
			if(boundaryCheck(nextPosisionX, nextPosisionY)){
				minDistance = nextDistance;
				nextDirection = i;
			}
		}
	}
	return nextDirection;
}

int Character::preyAI(){
	float rivalPositionX = rival->positionX;
	float rivalPositionY = rival->positionY;
	int nextDirection;
	nextDirection = directionPridiction(rivalPositionX, rivalPositionY);
	//nextDirection = nextDirectionPridiction(rivalPositionX, rivalPositionY);
	return nextDirection;
}

//predict the next direction according to the rival position
int Character::directionPridiction(float rpx, float rpy){
	int nextDirection = 4;
	int counterDirection;
	float maxDistance = 0;
	float nextDistance;
	float nextPosisionX;
	float nextPosisionY;

	counterDirection = getCounterDirection(this->direction);

	for(int i=0; i<4; i++){
		nextPosisionX = (this->positionX + getSpeedX(i));
		nextPosisionY = (this->positionY + getSpeedY(i));
		nextDistance = getRivalDistance(nextPosisionX, nextPosisionY);

		if(nextDistance >= maxDistance){
			if(boundaryCheck(nextPosisionX, nextPosisionY) && (i != rival->direction) && (i != counterDirection)){
				maxDistance = nextDistance;
				nextDirection = i;
			}
		}
	}
	return nextDirection;
}


void Character::movement(){
	//printf("direction is %d", direction);
	float nextPositionX;
	float nextPositionY;
	
	if(this->AI){
		if(this->predator){
			this->direction = predatorAI();
		}
		else{
			this->direction = preyAI();
		}
	}
	
	switch(this->direction){
		case 0:
			nextPositionY = (this->positionY + this->speedY);
			if(boundaryCheck(this->positionX, nextPositionY)){
				this->positionY += this->speedY;
			}
			break;
		case 1:
			nextPositionY = (this->positionY - this->speedY);
			if(boundaryCheck(this->positionX, nextPositionY)){
				this->positionY -= this->speedY;
			}
			break;
		case 2:
			nextPositionX = (this->positionX - this->speedX);
			if(boundaryCheck(nextPositionX, this->positionY)){
				this->positionX -= this->speedX;
			}
			break;
		case 3:
			nextPositionX = (this->positionX + this->speedX);
			if(boundaryCheck(nextPositionX, this->positionY)){
				this->positionX += this->speedX;
			}
			break;
		case 4:
			break;
		default:
			printf("no move.");
	}
}

int Character::getCounterDirection(int dier){
	int counterDirection = 4;
	switch(dier){
		case 0:
			counterDirection = 1;
			break;
		case 1:
			counterDirection = 0;
			break;
		case 2:
			counterDirection = 3;
			break;
		case 3:
			counterDirection = 2;
			break;
	}
	return counterDirection;
}

bool Character::getJumpState(){
	return jumpState;
}