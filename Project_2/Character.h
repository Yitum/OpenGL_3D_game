class Character{
public:
	//variables
	GLfloat angle;
	bool AI;
	bool animation;
	float positionX, positionY, positionZ;
	int direction;
	float speedX, speedY;
	float radius;
	float boundaryX, boundaryY;
	int flat, subdiv;
	bool predator;

	//Functions
	Character(float positionX, float positionY, float positionZ, float radius, bool AI);
	void Character::render();
	void Character::keyboardProcess(int key, int x, int y);
	void Character::movement();
	void Character::setColor(float R, float G, float B);
	void Character::setLineColor(float R, float G, float B);
	int Character::getCounterDirection(int dier);
	void Character::jumpMovement();
	bool Character::getJumpState();
	void Character::enablePredator();
	void Character::disablePredator();
	void Character::setRival(Character *rival);
	void Character::setPosition(float px, float py);
	void Character::setDirection(int dirc);
	void Character::collision();
	void Character::OriginalState();
	void Character::setRotationAngle(float angle);


private:
	//variables
	float RGB_R, RGB_G, RGB_B;
	float Line_R, Line_G, Line_B;
	float originalZPosition;
	bool jumpState;
	float bounce;
	float bounceTime;
	int jumpAngle;
	Character *rival;
	float rotationAngle;


	//Functions
	void init();
	void Character::Normalize(GLfloat v[3]);
	void Character::subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth);
	void Character::drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]);
	void Character::drawTriangleSmooth(float v1[3], float v2[3], float v3[3]);
	void Character::normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]);
	void Character::normCrossProd(float u[3], float v[3], float n[3]);
	bool Character::boundaryCheck(float npx, float npy);
	void Character::Jump();
	float Character::getRivalDistance(float npx, float npy);
	int Character::predatorAI();
	int Character::directionPridictionPredator(float rpx, float rpy);
	int Character::preyAI();
	int Character::directionPridiction(float rpx, float rpy);
	float Character::getSpeedX(int dire);
	float Character::getSpeedY(int dire);

};