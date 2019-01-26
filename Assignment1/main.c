/*******************************************************************
		   Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"

const int meshSize = 16;    // Default Mesh Size
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

// A quad mesh representing the ground
static QuadMesh groundMesh;

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

//spins propellers
void spinDisplay(void);

//draw donut
void drawDonut();

//drawing the drone
void drawDrone();
void drawArmBR();
void drawArmFR();
void drawArmFL();
void drawArmBL();

//drone measurements
float droneX = 6.0;
float droneY = 1.0;
float droneZ = 4.0;

//drone translation
float moveX = 0.0;
float moveY = 4.0;
float moveZ = 0.0;
float turnX = 45.0;

static GLfloat spinTheta = 0.0;

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 1");

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

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	// Set up ground quad mesh
	Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	//Set(&BBox.min, -8.0f, 0.0, -8.0);
	//Set(&BBox.max, 8.0f, 6.0,  8.0);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Drone

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

	// Apply transformations to move drone
	// ...

	// Apply transformations to construct drone, modify this!

	drawDrone();
	drawDonut();

	// Draw ground mesh
	DrawMeshQM(&groundMesh, meshSize);

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawDonut()
{
	glPushMatrix();
		glTranslatef(0.0, 5.0, -5.0);
		glutSolidTorus(0.5,10.0,50.0,50.0);
	glPopMatrix();
}

//draws drone
void drawDrone()
{	//CTM = I
	glPushMatrix();
		// p' = I * T(moveX, moveY, moveZ) * p
		glTranslatef(moveX, moveY, moveZ);
		// p' = I * T(moveX, moveY, moveZ) * R(turnX, 0.0, 1.0, 0.0) * p
		glRotatef(turnX, 0.0, 1.0, 0.0);
		// p' = I * T(moveX, moveY, moveZ) * R(turnX, 0.0, 1.0, 0.0) * S(droneX, droneY, droneZ) * p
		glScalef(droneX, droneY, droneZ);
		glutSolidCube(1.0);
		//making scale, rotatation and translation back to default
		glScalef(1 / droneX, 1 / droneY, 1 / droneZ);
		glRotatef(-45.0, 0.0, -1.0, 0.0);
		glTranslatef(0.0, -4.0, 0.0);
		//draw all the arms
		drawArmBR();
		drawArmFR();
		drawArmFL();
		drawArmBL();
	//after popping the matrix, CTM = I
	glPopMatrix();
}
//draws back right arm of drone
void drawArmBR()
{	//CTM = I
	glPushMatrix();
		// p' = I * T(1.0, 4.0, 4.0) * p
		glTranslatef(1.0, 4.0, 4.0);
		// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * p
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * S(droneX*0.5, droneY, droneZ*0.25) * p
		glScalef(droneX*0.5, droneY, droneZ*0.25);
		glutSolidCube(1.0);
		//inverse of the scale
		glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
		glPushMatrix();
			// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * p
			glTranslatef(-1.0, 0.5, 0.0);
			glutSolidCube(0.75);
			glPushMatrix();
				// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * p
				glRotatef(spinTheta, 0.0, 1.0, 0.0);
				// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * p
				glScalef(6.0, 0.5, 0.5);
				// p' = I * T(1.0, 4.0, 4.0) * R(90.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * T(0.0, 1.0, 0.0) * p
				glTranslatef(0.0, 1.0, 0.0);
				glutSolidCube(0.5);
			// CTM = I * T1 * R1 * T2 * p
			glPopMatrix();
		// CTM = I * T1 * R1 * p
		glPopMatrix();
	// CTM = I 
	glPopMatrix();
}
//draws front right arm of drone
void drawArmFR()
{	// CTM = I 
	glPushMatrix();
		// p' = I * T(-4.0, 4.0, -1.0) * p
		glTranslatef(-4.0, 4.0, -1.0);
		// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * p
		glRotatef(0.0, 0.0, 1.0, 0.0);
		// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * S(droneX*0.5, droneY, droneZ*0.25) * p
		glScalef(droneX*0.5, droneY, droneZ*0.25);
		glutSolidCube(1.0);
		//inverse the scale
		glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
		glPushMatrix();
			// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * p
			glTranslatef(-1.0, 0.5, 0.0);
			glutSolidCube(0.75);
			glPushMatrix();
				// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * p
				glRotatef(spinTheta, 0.0, 1.0, 0.0);
				// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * p
				glScalef(6.0, 0.5, 0.5);
				// p' = I * T(-4.0, 4.0, -1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * T(0.0, 1.0, 0.0) * p
				glTranslatef(0.0, 1.0, 0.0);
				glutSolidCube(0.5);
			// CTM = I * T1 * R1 * T2 * p
			glPopMatrix();
		// CTM = I * T1 * R1 * p
		glPopMatrix();
	// CTM = I 
	glPopMatrix();
}
//draws back left arm of drone
void drawArmBL()
{	// CTM = I 
	glPushMatrix();
		// p' = I * T(4.0, 4.0, 1.0) * p
		glTranslatef(4.0, 4.0, 1.0);
		// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * p
		glRotatef(0.0, 0.0, 1.0, 0.0);
		// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * S(droneX*0.5, droneY, droneZ*0.25) * p
		glScalef(droneX*0.5, droneY, droneZ*0.25);
		glutSolidCube(1.0);
		//inverse the scale
		glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
		glPushMatrix();
			// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * T(1.0, 0.5, 0.0) * p
			glTranslatef(1.0, 0.5, 0.0);
			glutSolidCube(0.75);
			glPushMatrix();
				// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * T(1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * p
				glRotatef(spinTheta, 0.0, 1.0, 0.0);
				// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * p
				glScalef(6.0, 0.5, 0.5);
				// p' = I * T(4.0, 4.0, 1.0) * R(0.0, 0.0, 1.0, 0.0) * T(-1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * T(0.0, 1.0, 0.0) * p
				glTranslatef(0.0, 1.0, 0.0);
				glutSolidCube(0.5);
			// CTM = I * T1 * R1 * T2 * p
			glPopMatrix();
		// CTM = I * T1 * R1 * p
		glPopMatrix();
	// CTM = I 
	glPopMatrix();
}
//draws front left arm of drone
void drawArmFL()
{	// CTM = I
	glPushMatrix();
		// p' = I * T(-1.0, 4.0, -4.0) * p
		glTranslatef(-1.0, 4.0, -4.0);
		// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) * p
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) * S(droneX*0.5, droneY, droneZ*0.25) * p
		glScalef(droneX*0.5, droneY, droneZ*0.25);
		glutSolidCube(1.0);
		//inverse the scale
		glScalef(1 / (droneX * 0.5), 1 / droneY, 1 / (droneZ * 0.25));
		glPushMatrix();
			// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) *  T(1.0, 0.5, 0.0) * p
			glTranslatef(1.0, 0.5, 0.0);
			glutSolidCube(0.75);
			glPushMatrix();
				// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) *  T(1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * p
				glRotatef(spinTheta, 0.0, 1.0, 0.0);
				// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) *  T(1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * p
				glScalef(6.0, 0.5, 0.5);
				// p' = I * T(-1.0, 4.0, -4.0) * R(90.0, 0.0, 1.0, 0.0) *  T(1.0, 0.5, 0.0) * R(spinTheta, 0.0, 1.0, 0.0) * S(6.0, 0.5, 0.5) * T(0.0, 1.0, 0.0) * p
				glTranslatef(0.0, 1.0, 0.0);
				glutSolidCube(0.5);
			// CTM = I * T1 * R1 * T2 * p
			glPopMatrix();
		// CTM = I * T1 * R1 * p
		glPopMatrix();
	// CTM = I
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
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	//to move drone back
	case 'b':
		moveX += 1.0 * sinf((turnX + 90.0f) * 3.14 / 180);
		moveZ += 1.0 * cosf((turnX + 90.0f) * 3.14 / 180);
		glutIdleFunc(spinDisplay);
		break;
	//to move drone forward
	case 'f': 
		moveX -= 1.0 * sinf((turnX + 90.0f) * 3.14 / 180);
		moveZ -= 1.0 * cosf((turnX + 90.0f) * 3.14 / 180);
		glutIdleFunc(spinDisplay);
		break;
	//to turn drone to the right
	case 'r': turnX += 1.0;
		break;
	//to turn drone to the left
	case 'l': turnX -= 1.0;
		break;
	//to spin propellers
	case 's': 
		glutIdleFunc(spinDisplay);
		break;
	case 't':
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("***INSTRUCTIONS***\n");
		printf("Press the arrow keys to move drone left, right, up and down\n");
		printf("Press F to move drone forward\nPress B to move drone backwards\n");
		printf("Press R to turn drone to the right\nPress L to turn drone to the left\n");
		printf("Press S to spin the propellers\n");
	}
	//transformations with arrow keys
	else if (key == GLUT_KEY_RIGHT)
	{
		moveX += 0.5;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		moveX -= 0.5;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		moveY -= 0.5;
	}
	else if (key == GLUT_KEY_UP)
	{
		moveY += 0.5;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
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

