/*******************************************************************
Multi - Part Model Construction and Manipulation
********************************************************************/

#ifdef _APPLE_
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Vector3D.c"
#include "QuadMesh.c"
#include "CubeMesh.c"

//Variables for creating textures
int texWidth;
int texHeight;
int nrChannels;

//Texture variables for each required texture
static unsigned int droneTex;
static unsigned int buildingTex;
static unsigned int streetTex;

const int meshSize = 40;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

//Street
//static GLfloat street_diffuse[] = { 169.0F, 169.0F, 169.0F, 1.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;
bool third = false;
bool first = false;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);

void recomputeOrientation(void);

//spins propellers
void spinDisplay(void);

//draw buildings
void drawBuildings(void);

//draw streets
void drawStreet(void);

//bullet stuff
void shoot(void);
void hit(void);

//drawing the drone
void drawDrone();
void drawArmBR();
void drawArmFR();
void drawArmFL();
void drawArmBL();
void drawEnemyDrone(void);

//drone measurements
float droneX = 1.0;
float droneY = 0.25;
float droneZ = 1.0;


//drone translation
float moveX = 5.0;
float moveY = 4.0;
float moveZ = 4.0;
float turnX = -90.0;

//enemy drone translation
float eMoveX = 1.0;
float eMoveY = 7.0;
float eMoveZ = 2.0;

float bulletMoveX = 5.0;
float bulletMoveY = 4.0;
float bulletMoveZ = 4.0;

//camera stuff
float x, y;
float z = 22;
GLdouble zoom = 60;
GLdouble thirdX;
GLdouble thirdY;
GLdouble thirdZ;
GLdouble firstX;
GLdouble firstY;
GLdouble firstZ;
GLdouble tiltVal = 0;

//building numbers
int buildingNum = 0;

//bool stuff
bool brokenDrone = false;
bool brokenEnemy = false;
bool aim = false;
bool crashed(void);

//array of "bounding boxes"
double boundingBoxArray[200];

GLint leftMouseButton, rightMouseButton;
int mouseX = 0, mouseY = 0;
float cameraTheta, cameraPhi, cameraRadius;

static GLfloat spinTheta = 0.0;

static CubeMesh buildingMesh, droneMesh, bulletMesh, streetMesh;

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 3");

	cameraRadius = 17.5f;
	cameraTheta = 2.80;
	cameraPhi = 2.0;
	recomputeOrientation();

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}

// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);   // This light is currently off
    unsigned char *dronePicture = stbi_load("goldDrone.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (dronePicture)
    {
        glGenTextures(1, &droneTex);
        glBindTexture(GL_TEXTURE_2D, droneTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, dronePicture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
    }
    
    stbi_image_free(dronePicture);
    
    unsigned char *buildingPicture = stbi_load("glassbuilding.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (buildingPicture)
    {
        glGenTextures(1, &buildingTex);
        glBindTexture(GL_TEXTURE_2D, buildingTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, buildingPicture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
        
    }
    
    stbi_image_free(buildingPicture);
    
    unsigned char *streetPicture = stbi_load("street.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (streetPicture)
    {
        glGenTextures(1, &streetTex);
        glBindTexture(GL_TEXTURE_2D, streetTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, buildingPicture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
    }
    
    stbi_image_free(streetPicture);

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	// Set up ground quad mesh
	Vector3D origin = NewVector3D(-20.0f, 0.0f, 20.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 40.0, 40.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    buildingMesh = newCube();
    droneMesh = newCube();
    bulletMesh = newCube();
    streetMesh = newCube();

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	//Set(&BBox.min, -8.0f, 0.0, -8.0);
	//Set(&BBox.max, 8.0f, 6.0,  8.0);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	//camera stuff
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Drone

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

	glutPostRedisplay();

	if (third)
	{
		thirdX = moveX + (5 * sinf((turnX * 3.14159) / 180));
		thirdZ = moveZ + (5 * cosf((turnX * 3.14159) / 180));

		gluLookAt(thirdX + 5.0, moveY + 1.0, thirdZ + 5.0, moveX, moveY, moveZ, 0.0, 1.0, 0.0);
	}

	else if (first)
	{
		firstY = moveY + tiltVal;
		firstX = (moveX - (5 * sinf((turnX * 3.14) / 180)));
		firstZ = (moveZ - (5 * cosf((turnX * 3.14) / 180)));

		gluLookAt(moveX, moveY + 0.5, moveZ, firstX - 5.0, firstY, firstZ - 5.0, 0.0, 1.0, 0.0);
	}
	else
	{
		gluLookAt(x, y, z, 0, 0, 0, 0, 1.0, 0);
	}

	brokenDrone = crashed();
	if (brokenDrone == false)
	{
		drawDrone();
	}

	if (brokenEnemy == false)
	{
		drawEnemyDrone();
	}

	if (aim)
	{
		for (int i = 0; i < 100; i++)
		{
			hit();
			shoot();
		}
		aim = false;
	}

	glutPostRedisplay();

	drawBuildings();
	drawStreet();
	glutIdleFunc(spinDisplay);

	// Draw ground mesh
	DrawMeshQM(&groundMesh, meshSize);

	glutSwapBuffers();   // Double buffering, swap buffers
}

void recomputeOrientation(void)
{
	x = cameraRadius * sinf(cameraTheta)*sinf(cameraPhi);
	z = cameraRadius * -cosf(cameraTheta)*sinf(cameraPhi);
	y = cameraRadius * -cosf(cameraPhi);
	glutPostRedisplay();
}

void shoot(void)
{
	bulletMoveX -= 0.1 * sinf((turnX + 90.0f) * 3.14 / 180);
	bulletMoveZ -= 0.1 * cosf((turnX + 90.0f) * 3.14 / 180);
	bulletMoveY = moveY;
	glPushMatrix();
	glTranslatef(bulletMoveX, bulletMoveY, bulletMoveZ);
	glScalef(droneX*0.05, droneY*0.25, droneZ*0.05);
	drawCube(&bulletMesh,droneTex);
	glPopMatrix();
}

void drawBuildings(void)
{
	buildingNum = 11;

	glPushMatrix();
	glTranslatef(7.0, 4.0, -7.0);
	glScalef(2.0, 4.0, 2.0);
	drawCube(&buildingMesh,buildingTex);
	glPopMatrix();

	boundingBoxArray[0] = 5.0;
	boundingBoxArray[1] = 9.0;
	boundingBoxArray[2] = -9.0;
	boundingBoxArray[3] = -5.0;
	boundingBoxArray[4] = 8.0;

	glPushMatrix();
	glTranslatef(-7.0, 4.0, -7.0);
	glScalef(2.0, 7.0, 2.0);
	drawCube(&buildingMesh,buildingTex);
	glPopMatrix();

	boundingBoxArray[5] = -9.0;
	boundingBoxArray[6] = -5.0;
	boundingBoxArray[7] = -9.0;
	boundingBoxArray[8] = -5.0;
	boundingBoxArray[9] = 11.0;

	glPushMatrix();
	glTranslatef(-5.0, 4.0, -2.0);
	glScalef(4.0, 5.0, 2.0);
	drawCube(&buildingMesh,buildingTex);
	glPopMatrix();

	boundingBoxArray[10] = -9.0;
	boundingBoxArray[11] = -1.0;
	boundingBoxArray[12] = -4.0;
	boundingBoxArray[13] = 0.0;
	boundingBoxArray[14] = 9.0;
    
    glPushMatrix();
    glTranslatef(17.0, 4.0, -17.0);
    glScalef(2.0, 10.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[15] = 15.0;
    boundingBoxArray[16] = 19.0;
    boundingBoxArray[17] = -19.0;
    boundingBoxArray[18] = -15.0;
    boundingBoxArray[19] = 14.0;
    
    glPushMatrix();
    glTranslatef(12.0, 4.0, -17.0);
    glScalef(2.0, 10.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[20] = 10.0;
    boundingBoxArray[21] = 14.0;
    boundingBoxArray[22] = -19.0;
    boundingBoxArray[23] = -15.0;
    boundingBoxArray[24] = 14.0;

    glPushMatrix();
    glTranslatef(14.0, 4.0, -11.0);
    glScalef(4.0, 5.0, 4.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[25] = 10.0;
    boundingBoxArray[26] = 18.0;
    boundingBoxArray[27] = -15.0;
    boundingBoxArray[28] = -7.0;
    boundingBoxArray[29] = 9.0;
    
    glPushMatrix();
    glTranslatef(-17.0, 4.0, -17.0);
    glScalef(2.5, 7.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[30] = -19.5;
    boundingBoxArray[31] = -14.5;
    boundingBoxArray[32] = -19.0;
    boundingBoxArray[33] = -15.0;
    boundingBoxArray[34] = 11.0;
    
    glPushMatrix();
    glTranslatef(-17.0, 4.0, -12.0);
    glScalef(2.5, 5.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[35] = -19.5;
    boundingBoxArray[36] = -14.5;
    boundingBoxArray[37] = -14.0;
    boundingBoxArray[38] = -10.0;
    boundingBoxArray[39] = 9.0;
    
    glPushMatrix();
    glTranslatef(-17.0, 4.0, 17.0);
    glScalef(2.0, 4.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[40] = -19.0;
    boundingBoxArray[41] = -15.0;
    boundingBoxArray[42] = 15.0;
    boundingBoxArray[43] = 19.0;
    boundingBoxArray[44] = 8.0;
    
    glPushMatrix();
    glTranslatef(16.0, 4.0, 15.0);
    glScalef(4.0, 4.0, 5.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[45] = 12.0;
    boundingBoxArray[46] = 20.0;
    boundingBoxArray[47] = 10.0;
    boundingBoxArray[48] = 20.0;
    boundingBoxArray[49] = 8.0;
    
    glPushMatrix();
    glTranslatef(10.0, 4.0, -1.0);
    glScalef(5.0, 3.0, 2.0);
    drawCube(&buildingMesh,buildingTex);
    glPopMatrix();
    
    boundingBoxArray[50] = 5.0;
    boundingBoxArray[51] = 15.0;
    boundingBoxArray[52] = -3.0;
    boundingBoxArray[53] = 1.0;
    boundingBoxArray[54] = 7.0;
}
//draws enemy drone
void drawEnemyDrone(void)
{
	CubeMesh drone;
	glPushMatrix();
	glTranslatef(eMoveX, eMoveY, eMoveZ);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.25, 1.0);
	drawCube(&drone,droneTex);
	glScalef(1 / droneX, 1 / droneY, 1 / droneZ);
	glRotatef(-45.0, 0.0, -1.0, 0.0);
	glTranslatef(0.0, -4.0, 0.0);
	drawArmBR();
	drawArmFR();
	drawArmFL();
	drawArmBL();
	glPopMatrix();
}

//draws drone
void drawDrone()
{
	CubeMesh body;
	glPushMatrix();
	glTranslatef(moveX, moveY, moveZ);
	glRotatef(turnX, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.25, 1.0);
	drawCube(&body,droneTex);
	glScalef(1 / droneX, 1 / droneY, 1 / droneZ);
	glRotatef(-45.0, 0.0, -1.0, 0.0);
	glTranslatef(0.0, -4.0, 0.0);
	drawArmBR();
	drawArmFR();
	drawArmFL();
	drawArmBL();
	glPopMatrix();
}
//draws back right arm of drone
void drawArmBR()
{
	CubeMesh arm;
	CubeMesh top;
	CubeMesh propeller;

	glPushMatrix();
	glTranslatef(0.0, 4.0, 1.5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(droneX*0.5, droneY, droneZ*0.25);
	drawCube(&arm,droneTex);
	glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
	glPushMatrix();
	glTranslatef(-0.25, 0.25, 0.0);
	glScalef(droneX*0.25, droneY - 0.5, droneZ*0.25);
	drawCube(&top,droneTex);
	glScalef(1 / (droneX * 0.25), 1 / (droneY - 0.5), 1 / (droneZ * 0.25));
	glPushMatrix();
	glRotatef(spinTheta, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.05, 0.1);
	glTranslatef(0.0, 5.0, 0.0);
	drawCube(&propeller,droneTex);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

}
//draws front right arm of drone
void drawArmFR()
{
	CubeMesh arm;
	CubeMesh top;
	CubeMesh propeller;

	glPushMatrix();
	glTranslatef(-1.5, 4.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	glScalef(droneX*0.5, droneY, droneZ*0.25);
	drawCube(&arm,droneTex);
	glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
	glPushMatrix();
	glTranslatef(-0.25, 0.25, 0.0);
	glScalef(droneX*0.25, droneY - 0.5, droneZ*0.25);
	drawCube(&top,droneTex);
	glScalef(1 / (droneX * 0.25), 1 / (droneY - 0.5), 1 / (droneZ * 0.25));
	glPushMatrix();
	glRotatef(spinTheta, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.05, 0.1);
	glTranslatef(0.0, 5.0, 0.0);
	drawCube(&propeller,droneTex);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}
//draws back left arm of drone
void drawArmBL()
{
	CubeMesh arm;
	CubeMesh top;
	CubeMesh propeller;

	glPushMatrix();
	glTranslatef(1.5, 4.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	glScalef(droneX*0.5, droneY, droneZ*0.25);
	drawCube(&arm,droneTex);
	glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
	glPushMatrix();
	glTranslatef(0.25, 0.25, 0.0);
	glScalef(droneX*0.25, droneY - 0.5, droneZ*0.25);
	drawCube(&top,droneTex);
	glScalef(1 / (droneX * 0.25), 1 / (droneY - 0.5), 1 / (droneZ * 0.25));
	glPushMatrix();
	glRotatef(spinTheta, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.05, 0.1);
	glTranslatef(0.0, 5.0, 0.0);
	drawCube(&propeller,droneTex);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}
//draws front left arm of drone
void drawArmFL()
{
	CubeMesh arm;

	glPushMatrix();
	glTranslatef(0.0, 4.0, -1.5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(droneX*0.5, droneY, droneZ*0.25);
	drawCube(&arm,droneTex);
	glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
	glPushMatrix();
	glTranslatef(0.25, 0.25, 0.0);
	glScalef(droneX*0.25, droneY - 0.5, droneZ*0.25);
	drawCube(&arm,droneTex);
	glScalef(1 / (droneX * 0.25), 1 / (droneY - 0.5), 1 / (droneZ * 0.25));
	glPushMatrix();
	glRotatef(spinTheta, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.05, 0.1);
	glTranslatef(0.0, 5.0, 0.0);
	drawCube(&arm,droneTex);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawStreet(void)
{
    glPushMatrix();
    glTranslatef(2.0, 0.1, 0.0);
    glScalef(2.0, 0.1, 20.0);
    drawCube(&streetMesh,streetTex);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-11.0, 0.1, 0.0);
    glScalef(2.0, 0.1, 20.0);
    drawCube(&streetMesh,streetTex);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.1, 8.0);
    glScalef(20.0, 0.1, 2.0);
    drawCube(&streetMesh,streetTex);
    glPopMatrix();
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode -
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(zoom, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	//gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		//to move drone back
	case 'b':
		crashed();
		moveX += 1.0 * sinf((turnX + 90.0f) * 3.14 / 180);
		moveZ += 1.0 * cosf((turnX + 90.0f) * 3.14 / 180);
		glutIdleFunc(spinDisplay);
		break;

		//to move drone forward
	case 'f':
		crashed();
		moveX -= 1.0 * sinf((turnX + 90.0f) * 3.14 / 180);
		moveZ -= 1.0 * cosf((turnX + 90.0f) * 3.14 / 180);
		glutIdleFunc(spinDisplay);
		break;

		//to turn drone to the right
	case 'l':
		turnX += 2.0;
		break;

		//to turn drone to the left
	case 'r':
		turnX -= 2.0;
		break;

		//to spin propellers
	case 's':
		glutIdleFunc(spinDisplay);
		break;

	case 'u':
		//zoom in
		zoom = zoom - 3;
		reshape(vWidth, vHeight);
		break;

	case 'j':
		//zoom out
		zoom = zoom + 3;
		reshape(vWidth, vHeight);
		break;
            
    case 'i':
        tiltVal += 0.5;
        break;
            
    case 'k':
        tiltVal -= 0.5;
        break;
            
	case ' ':
		bulletMoveX = moveX;
		bulletMoveY = moveY;
		bulletMoveZ = moveZ;
		aim = true;
		break;

		// Help key
	case 'h':
		printf("***INSTRUCTIONS***\n");
		printf("Press the arrow keys to move drone left, right, up and down\n");
		printf("Press F to move drone forward\nPress B to move drone backwards\n");
		printf("Press R to turn drone to the right\nPress L to turn drone to the left\n");
		printf("Press F1 for third person view\n");
		printf("Press F2 for first person view\n");
		printf("Press spacebar to shoot bullet (bullet moves super fast!)\n");
		printf("IN THE DEFAULT VIEW MODE:\n");
		printf("Press U to zoom into view\n");
		printf("Press J to zoom out of view\n");
        printf("Use left-click on mouse to drag the view\n");
        printf("IN THE FIRST PERSON VIEW:\n");
        printf("Press I to tilt camera up\n");
        printf("Press K to tilt camera down\n");
		break;

	case 't':
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	//transformations with arrow keys
	if (key == GLUT_KEY_RIGHT)
	{
		crashed();
		moveX += 0.5;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		crashed();
		moveX -= 0.5;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		crashed();
		moveY -= 0.5;
	}
	else if (key == GLUT_KEY_UP)
	{
		crashed();
		moveY += 0.5;
	}

	else if (key == GLUT_KEY_F1)
	{
		third = true;
		first = false;
	}
	else if (key == GLUT_KEY_F2)
	{
		first = true;
		third = false;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to
void mouse(int button, int state, int thisX, int thisY)
{
	//update the left and right mouse button states
	if (button == GLUT_LEFT_BUTTON)
		leftMouseButton = state;
	else if (button == GLUT_RIGHT_BUTTON)
		rightMouseButton = state;

	//update the last seen X and Y coordinates of the mouse
	mouseX = thisX;
	mouseY = thisY;
}

// Mouse motion callback - use only if you want to
void mouseMotionHandler(int x, int y)
{
	if (leftMouseButton == GLUT_DOWN)
	{
		cameraTheta += (x - mouseX)*0.005;
		cameraPhi += (y - mouseY)*0.005;

		if (cameraPhi <= 0)
			cameraPhi = 0 + 0.001;
		if (cameraPhi >= 3.141)
			cameraPhi = 3.141 - 0.001;

		recomputeOrientation();
	}
	else if (rightMouseButton == GLUT_DOWN) {
		double totalChangeSq = (x - mouseX) + (y - mouseY);

		cameraRadius += totalChangeSq * 0.01;

		if (cameraRadius < 2.0)
			cameraRadius = 2.0;
		if (cameraRadius > 10.0)
			cameraRadius = 10.0;

		recomputeOrientation();
	}
	mouseX = x;
	mouseY = y;
}


Vector3D ScreenToWorld(int x, int y)
{
	// you will need to finish this if you use the mouse
	return NewVector3D(0, 0, 0);
}

//handles the spinning of the propellers
void spinDisplay(void)
{
	spinTheta += 1.5;
	if (spinTheta > 360.0)
		spinTheta -= 360.0;

	glutPostRedisplay();
}

bool crashed(void)
{
	if (moveY <= 0.5)
	{
		moveY = -100;
		return true;
	}

	for (int i = 0; i < buildingNum; i++)
	{
		if (moveX >= boundingBoxArray[0 + (5 * i)] && moveX <= boundingBoxArray[1 + (5 * i)] && moveZ >= boundingBoxArray[2 + (5 * i)] && moveZ <= boundingBoxArray[3 + (5 * i)] && moveY <= boundingBoxArray[4 + (5 * i)])
		{
			moveY = -100;
			return true;
		}
	}
	return false;
}

void hit(void)
{
	if (bulletMoveX <= (eMoveX + 1) && bulletMoveX >= (eMoveX - 1) && (bulletMoveY == eMoveY) && bulletMoveZ <= (eMoveZ + 1) && bulletMoveZ >= (eMoveZ - 1))
	{
		brokenEnemy = true;
	}
}
