
#include <Stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Character.h"
#include "Obstacle.h"

//#pragma comment (lib, "glew32.lib")
#pragma comment(lib,"glew32.lib")
//#define random(x) (rand()%x)

float cameraX, cameraY, cameraZ;
float cameraOffsetX, cameraOffsetY, cameraOffsetZ;
float lookAtX, lookAtY, lookAtZ;
float lookAtOffsetX, lookAtOffsetY, lookAtOffsetZ;
float boundaryX, boundaryY;
int obstacleNumber = 2;
float distanceBuget = 3.8f;
float realtimeDistance;
float originalPositionX = -1.0f;
float originalPositionY = 0;
float originalRotationAngle = 1.0f;
float rotationAngle;
bool gameState = true;
bool lightState = true;
int lightStyle = 0;
int scorePlayer = 0;
int scoreAI = 0;
int lightColor = 1;
float lightPosX = -1.0f, lightPosY = -0.5f, lightPosZ = 0.1f;

//float cameraX = 0.0f, cameraY = -12.0f, cameraZ = 0.2f;
//float lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f; 

Character *player1;
Character *player2;
Obstacle *obstacles[2];

float random(int range){
	float randomNumber;
	randomNumber = rand()%range *0.01f;
	return randomNumber;
}

void renderBitmapString(float x, float y, void *font, char *string){  
  char *c;
  //glRasterPos2f(x,y);
  glWindowPos2f(x,y);
  for (c=string; *c != '\0'; c++){
    glutBitmapCharacter(font, *c);
  }
}

void renderText(void){
	char buffer1[256];
	char buffer2[256];
	char buffer3[256];

	glColor3f(1.0f, 0.5f, 0.0f);
	sprintf(buffer1, "%s : %f", "Distance Budget: ", distanceBuget);
	sprintf(buffer2, "%s : %f", "Real Time Distance : ", realtimeDistance);
	sprintf(buffer3, "%s : %d : %s : %d", "Player: ", scorePlayer, " AI: ", scoreAI);
	/*renderBitmapString(-3.0f, 3.5f, GLUT_BITMAP_9_BY_15, buffer1);
	renderBitmapString(-3.0f, 3.0f, GLUT_BITMAP_9_BY_15, buffer2);
	renderBitmapString(1.0f, 3.0f, GLUT_BITMAP_9_BY_15, buffer3);*/
	renderBitmapString(20.0f, 750.0f, GLUT_BITMAP_9_BY_15, buffer1);
	renderBitmapString(20.0f, 730.0f, GLUT_BITMAP_9_BY_15, buffer2);
	renderBitmapString(780.0f, 750.0f, GLUT_BITMAP_9_BY_15, buffer3);

}

void renderPrimitive(void){
	glColor3f(0, 0.3, 0.15f);
	glBegin(GL_QUADS);
		glVertex3f(-boundaryX, -boundaryY, 0.0f); // The bottom left corner
		glVertex3f(-boundaryX, boundaryY, 0.0f); // The top left corner
		glVertex3f(boundaryX, boundaryY, 0.0f); // The top right corner
		glVertex3f(boundaryX, -boundaryY, 0.0f); // The bottom right corner
	glEnd();
}

void renderScene(void){
	GLfloat R, G, B;
	GLfloat ambientcolor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientcolor);

	//pointed light
	GLfloat lightColor0[] = {0.7f, 0.7f, 0.7f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPosition0[] = {4.0f, 0.0f, 5.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);

	
	//directional light
	if(lightColor == 1){
		R = 0.5f;
		G = 0.5f;
		B = 0.0f;
	}
	else if(lightColor == 2){
		R = 0.15f;
		G = 0.7f;
		B = 1.0f;
	}
	else if(lightColor == 3){
		R = 0.5f;
		G = 1.0f;
		B = 0.15f;
	}
	GLfloat lightColor1[] = {R, G, B, 0.0f};
	GLfloat lightPosition1[] = {lightPosX, lightPosY, lightPosZ, 0.0f}; 
	if(lightStyle == 0){
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	}
	else{
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor1);
	}
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
}

void renderObject(void){
	int i;
	player1->render();
	player2->render();
	for(i=0;i<obstacleNumber;i++){
		obstacles[i]->render();
	}
}

void render(void){
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(0, 0, -6.0f);
	gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);

	renderPrimitive();
	renderScene();
	renderObject();
	renderText();
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height){
	if(height == 0){
		height = 1;
	}
	float ratio = width*1.0f / height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0 ,0, (GLsizei)width, (GLsizei)height);
	gluPerspective(50.0f, ratio, 0.1f, 100.0f);
	//glOrtho(-1.25, 1.25, -1.25 , 1.25 , -2.0, 2.0);
	//glFrustum(-1.0,1.0,-1.0,1.0,1.5,20.0);
	glMatrixMode(GL_MODELVIEW);
}

void initCharacter(){
	int i;
	player1 = new Character(-1.0f, 0.0f, 0.5f, 0.25f, false);
	player1->enablePredator();
	player2 = new Character(1.0f, 0.0f, 0.5f, 0.25f, true);
	player2->disablePredator();
	player2->setRival(player1);
	player2->setLineColor(0.0f, 1.0f, 0.0f);

	for(i=0;i<obstacleNumber;i++){
		float obstaclePositionX = random(660)-3.3f;
		float obstaclePositionY = random(500)-2.5f;
		obstacles[i] = new Obstacle(obstaclePositionX, obstaclePositionY, 0.3f, 0.1f);
	}
}

void updateView(){
	cameraX = player1->positionX+cameraOffsetX, cameraY = player1->positionY-3.5f+cameraOffsetY, cameraZ = 5.0f+cameraOffsetZ;
	lookAtX = player1->positionX+lookAtOffsetY, lookAtY = player1->positionY-0.25+lookAtOffsetX, lookAtZ = player1->positionZ+lookAtOffsetZ;
}

void initRender(){
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	if(lightState){
		glEnable(GL_LIGHTING);
	}
	else glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void init(){
	gameState = true;
	cameraOffsetX = 0.0f, cameraOffsetY = 0.0f, cameraOffsetZ = 0.0f;
	lookAtOffsetX = 0.0f, lookAtOffsetY = 0.0f, lookAtOffsetZ = 0.0f;
	initRender();
	initCharacter();
	updateView();
	boundaryX = 3.3f, boundaryY = 2.5f;
	rotationAngle = originalRotationAngle;
}


void changeCameraPosition(unsigned char key, int x, int y){
	switch(key){
		case 'a':
			cameraOffsetX += 0.1;
			break;
		case 'd':
			cameraOffsetX -= 0.1;
			break;
		case 'e':
			cameraOffsetZ -= 0.1;
			break;
		case 'q':
			cameraOffsetZ += 0.1;
			break;
		case 's':
			cameraOffsetY -= 0.1;
			break;
		case 'w':
			cameraOffsetY += 0.1;
			break;
	}

}

void changeLightStyle(void){
	if(lightStyle == 0){
		lightStyle = 1;
	}
	else lightStyle = 0;
	renderScene();
}

void keyPressed(unsigned char key, int x, int y){
	switch(key){
		case 'a':
			changeCameraPosition(key, x, y);
			break;
		case 'd':
			changeCameraPosition(key, x, y);
			break;
		case 'e':
			changeCameraPosition(key, x, y);
			break;
		case 'k':
			changeLightStyle();
			printf("light style is %d\n", lightStyle);
			break;
		case 'p':
			if(gameState){
				gameState = false;
			}
			else gameState =true;

			printf("Pause \n");
			break;
		case 'q':
			changeCameraPosition(key, x, y);
			break;
		case 'l':
			if(lightState){
				lightState = false;
			}
			else lightState = true;
			initRender();
			printf("switch light \n");
			break;
		case 's':
			changeCameraPosition(key, x, y);
			break;
		case 'w':
			changeCameraPosition(key, x, y);
			break;
		case 'x':
			lightPosZ++;
			break;
		case 'z':
			lightPosZ--;
			break;
		case 32:
			player1->keyboardProcess(key, x, y);
			break;
		case 49:
			lightColor = 1;
			break;
		case 50:
			lightColor = 2;
			break;
		case 51:
			lightColor = 3;
			break;
		case 'n':
			lookAtOffsetX += 0.1f;
			break;
		case 'm':
			lookAtOffsetX -= 0.1f;
			break;
	}

}

void specialkeyPressed(int key, int x, int y){
	if(gameState){
		switch (key){
		case GLUT_KEY_UP: 
			player1->keyboardProcess(key, x, y);
			break;
		case GLUT_KEY_DOWN: 
			player1->keyboardProcess(key, x, y);
			break;
		case GLUT_KEY_LEFT: 
			player1->keyboardProcess(key, x, y);
			break;
		case GLUT_KEY_RIGHT: 
			player1->keyboardProcess(key, x, y);
			break;
		}
	}
}


void repeatGame(void){
	player1->setPosition(originalPositionX, originalPositionY);
	player1->setDirection(4);
	player2->setPosition((originalPositionX + 2.0f), originalPositionY);
	player2->setDirection(4);
}

void collisionTrakle(void){
	player1->collision();
	player2->collision();
	distanceBuget = distanceBuget*1.01f; 
	rotationAngle += 5.0f;
	player1->setRotationAngle(rotationAngle);
	
	if(player1->predator){
		player1->direction = player1->getCounterDirection(player1->direction);
		scorePlayer++;
	}
	else{
		scoreAI++;
		repeatGame();
	}
	printf("collision happends \n");
}



void collisionCheck(void){
	float distanceXY, safeDistance;
	distanceXY = sqrt(pow((player1->positionX-player2->positionX), 2)+pow((player1->positionY-player2->positionY), 2));
	safeDistance = player1->radius+player2->radius;
	if(distanceXY < safeDistance){
		if(abs(player1->positionZ-player2->positionZ)<safeDistance){
			collisionTrakle();
		}
	}
}

void obstacleCollisionTrakle(int num){
	obstacles[num]->stop();
	if(player1->predator){
		player1->direction = player1->getCounterDirection(player1->direction);
		distanceBuget = distanceBuget*0.025f; 
	}
	else{
		distanceBuget = distanceBuget*1.025f;
	}
	printf("obstacle collision happends \n");
}

void obstacleCollisionCheck(void){
	int i;
	float distanceXO, safeDistance;
	for(i=0; i<obstacleNumber; i++){
		distanceXO = sqrt(pow((player1->positionX-obstacles[i]->positionX), 2) + pow((player1->positionY - obstacles[i]->positionY), 2));
		safeDistance = player1->radius+obstacles[i]->radius;
		if(distanceXO < safeDistance){
			if(abs(player1->positionZ - obstacles[i]->positionZ) < safeDistance){
				obstacleCollisionTrakle(i);
			}
		}
	}

}


void switchRole(void){
	if(player1->predator){
		player1->disablePredator();
	}
	else player1->enablePredator();

	if(player2->predator){
		player2->disablePredator();
	}
	else player2->enablePredator();
	printf("switch the role \n");
}

void restart(void){		
	int i;
	distanceBuget = 3.5f;
	player1->OriginalState();
	player2->OriginalState();
	for(i=0;i<obstacleNumber;i++){
		obstacles[i]->restart();
	}
}

void checkDistance(void){
	realtimeDistance = sqrt(pow((player1->positionX-player2->positionX), 2)+pow((player1->positionY-player2->positionY), 2));
	if(realtimeDistance > distanceBuget){
		Sleep(1000);
		switchRole();
		restart();
		repeatGame();
	}
}

void jumpCheck(int value){
	if(player1->getJumpState()){
		player1->jumpMovement();
	}
	glutPostRedisplay();
	glutTimerFunc(10, jumpCheck, 0);
}

void movement(){
	int i;
	if(gameState){
		player1->movement();
		player2->movement();
		for(i=0;i<obstacleNumber;i++){
			obstacles[i]->movement();
		}
	}
	

}

void update(int value){
	movement();
	updateView();
	collisionCheck();
	obstacleCollisionCheck();
	checkDistance();
	glutPostRedisplay();

	glutTimerFunc(10, update, 1);
}

void idle(){
	movement();
	updateView();
	collisionCheck();
	obstacleCollisionCheck();
	checkDistance();
	glutPostRedisplay();

}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("Project 2 Tong Wang");

	glewInit();
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialkeyPressed);
	//glutIdleFunc(idle);

	init();

	glutTimerFunc(10, jumpCheck, 0);
	glutTimerFunc(10, update, 1);

	glutMainLoop();

	return 1;
}