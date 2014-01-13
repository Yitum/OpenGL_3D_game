class Obstacle{

public:
	//Variables
	GLfloat angle;
	bool animation;
	float positionX, positionY, positionZ;
	int direction;
	float speedX, speedY;
	float radius;
	float boundaryX, boundaryY;
	int flat, subdiv;
	
	//Function
	Obstacle(float positionX, float positionY, float positionZ, float radius);
	void Obstacle::render();
	void Obstacle::movement();
	void Obstacle::stop();
	void Obstacle::restart();
	

private:
	//Variables
	float RGB_R, RGB_G, RGB_B;

	//Function
	void init();
	void Obstacle::Normalize(GLfloat v[3]);
	void Obstacle::subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth);
	void Obstacle::drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]);
	void Obstacle::drawTriangleSmooth(float v1[3], float v2[3], float v3[3]);
	void Obstacle::normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]);
	void Obstacle::normCrossProd(float u[3], float v[3], float n[3]);
	float Obstacle::random(int range);

};