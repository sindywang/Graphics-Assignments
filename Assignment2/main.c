/*******************************************************************
		   Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#pragma warning(disable:4996)

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

const int floorHeight = 1.0;  //height of each floor

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
static GLfloat street_diffuse[] = { 169.0F, 169.0F, 169.0F, 1.0F };

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

void createBlock(void);
void drawStreet(void);
void extrudedMesh(float sX, float sY, float sZ, float x, float y, float z);
void resetCube(void);

//read and write 
void save(float sX, float sY, float sZ, float x, float y, float z);
void read(void);

void recomputeOrientation(void);

//cube scale and translates
float moveX = -7.0;
float moveY = 1.0;
float moveZ = 7.0;

float scaleX = 1.0;
float scaleY = 1.0;
float scaleZ = 1.0;

//camera stuff
float x, y;
float z = 22;
GLdouble zoom = 60;

GLint leftMouseButton, rightMouseButton;
int mouseX = 0, mouseY = 0;
float cameraTheta, cameraPhi, cameraRadius;

//key setting for t,h,s,etc
int keySetting = 0;

int floors = 0;

//read values
float insX, insY, insZ, inx, iny, inz;

//index counter for building quads array
int place = 0;

//index counter for the array of buildings
int buildingIndex = 0;

bool drawBlock = false;
bool drawBase = false;
bool drawSaved = false;

typedef struct Vertex
{
	float point[3];
} Vertex;

typedef struct Quad
{
	Vertex index[4];
} Quad;

typedef struct Building
{
	Vertex vertices[200];
	Quad quads[200];

} Building;

void drawBuilding(Building buildings);
Building city[200];

int main(int argc, char **argv)
{

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 2");

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
	//camera stuff
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0, 0, 0, 0.0f, 1.0f, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

	//to draw starting cube
	if (drawBlock == true)
	{
		createBlock();
	}

	//to draw mesh
	if (drawBase == true)
	{
		extrudedMesh(scaleX, scaleY, scaleZ, moveX, moveY, moveZ);
	}

	//to save buildings
	if (drawSaved == true)
	{
		extrudedMesh(insX, insY, insZ, inx, iny, inz);
	}
	drawStreet();

	// Draw ground mesh
	DrawMeshQM(&groundMesh, meshSize);

	glutSwapBuffers();   // Double buffering, swap buffers
}

void recomputeOrientation()
{
	x = cameraRadius * sinf(cameraTheta)*sinf(cameraPhi);
	z = cameraRadius * -cosf(cameraTheta)*sinf(cameraPhi);
	y = cameraRadius * -cosf(cameraPhi);
	glutPostRedisplay();
}

void createBlock(void)
{

	glPushMatrix();
	glTranslatef(moveX, moveY, moveZ);
	glScalef(scaleX, scaleY, scaleZ);
	glutSolidCube(2.0);
	glPopMatrix();
}

void drawStreet(void)
{
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, street_diffuse);
	glTranslatef(0.0, 0.1, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-7.5, 0.0, 7.5);
	glVertex3f(7.5, 0.0, 7.5);
	glVertex3f(7.5, 0.0, -7.5);
	glVertex3f(-7.5, 0.0, -7.5);
	glEnd();
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
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':
		//translate setting for keys
		keySetting = 1;
		break;

	case 'h':
		//height setting for keys
		keySetting = 2;
		break;

	case 's':
		//scale X and Z setting for keys
		keySetting = 3;
		break;

	case 'e':
		//extrude the mesh and saves into file
		drawBase = true;
		drawBlock = false; 
		keySetting = 0;
		save(scaleX, scaleY, scaleZ, moveX, moveY, moveZ);
		break;

	case 'p':
		//reset the solid cube
		resetCube();
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
	}


	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		//place solid cube
		place = 0;
		drawBlock = true;
	}
	/*
	if (key == GLUT_KEY_F2)
	{
		//read from file
		read();
	}
	*/
	//translate cube
	if (keySetting == 1)
	{
		if (key == GLUT_KEY_LEFT)
		{
			moveX -= 0.5;
		}
		else if (key == GLUT_KEY_RIGHT)
		{
			moveX += 0.5;
		}
		else if (key == GLUT_KEY_UP)
		{
			moveZ -= 0.5;
		}
		else if (key == GLUT_KEY_DOWN)
		{
			moveZ += 0.5;
		}
	}

	//scale cube height
	if (keySetting == 2)
	{
		if (key == GLUT_KEY_UP)
		{
			scaleY += 0.5;
			//moveY += 0.5;
		}
		else if (key == GLUT_KEY_DOWN)
		{
			scaleY -= 0.5;
			//moveY -= 0.5;
		}
	}

	//scales x and z of cube
	if (keySetting == 3)
	{
		if (key == GLUT_KEY_UP)
		{
			scaleX += 0.5;
		}
		else if (key == GLUT_KEY_DOWN)
		{
			scaleX -= 0.5;
		}
		else if (key == GLUT_KEY_LEFT)
		{
			scaleZ -= 0.5;
		}
		else if (key == GLUT_KEY_RIGHT)
		{
			scaleZ += 0.5;
		}
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

int countFloors(void)
{
	float height = 2;  //scale of the cube is 2
	height += scaleY;
	floors = (height / floorHeight) - 1;  //minus 1 because of the initial scale to 1
	return floors;
}

void drawBuilding(Building buildings)
{
	//draws the buildings
	float x, y, z;
	Quad quad;

	for (int i = 0; i < (place + 1); i++)
	{
		quad = buildings.quads[i];
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++)
		{
			Vertex v = quad.index[j];
			x = v.point[0];
			y = v.point[1];
			z = v.point[2];
			glVertex3f(x, y, z);
		}
		glEnd();
	}
	place = -1;
}

void extrudedMesh(float sX, float sY, float sZ, float x, float y, float z)
{
	//makes extruded mesh for drawing
	Vertex vertex1, vertex2, vertex3, vertex4;
	Quad quads;
	Building buildings;

	floors = countFloors();

	//floors
	for (int i = 0; i < floors + 1; i++)
	{
		vertex1.point[0] = x - sX;
		vertex1.point[1] = 0.1 + (floorHeight * i);
		vertex1.point[2] = z + sZ;

		vertex2.point[0] = x + sX;
		vertex2.point[1] = 0.1 + (floorHeight * i);
		vertex2.point[2] = z + sZ;

		vertex3.point[0] = x + sX;
		vertex3.point[1] = 0.1 + (floorHeight * i);
		vertex3.point[2] = z - sZ;

		vertex4.point[0] = x - sX;
		vertex4.point[1] = 0.1 + (floorHeight * i);
		vertex4.point[2] = z - sZ;

		quads.index[0] = vertex1;
		quads.index[1] = vertex2;
		quads.index[2] = vertex3;
		quads.index[3] = vertex4;

		buildings.quads[place] = quads;
		place += 1;
	}

	//putting quads into vertices array in building
	for (int i = 0; i < floors + 1; i++)
	{
		Quad q = buildings.quads[i];
		for (int j = 0; j < 4; j++)
		{
			Vertex v = q.index[j];
			buildings.vertices[j + (i * 4)] = v;
		}
	}

	//walls
	//front wall
	for (int i = 1; i < floors + 1; i++)
	{
		Quad wall;
		int a = 0;
		int b = 1;

		wall.index[0] = buildings.vertices[a];
		wall.index[1] = buildings.vertices[b];
		wall.index[2] = buildings.vertices[b + (i * 4)];
		wall.index[3] = buildings.vertices[a + (i * 4)];

		buildings.quads[place] = wall;
		place += 1;
	}
	
	//right wall
	for (int i = 1; i < floors + 1; i++)
	{
		Quad wall;
		int a = 1;
		int b = 2;
		wall.index[0] = buildings.vertices[a];
		wall.index[1] = buildings.vertices[b];
		wall.index[2] = buildings.vertices[b + (i * 4)];
		wall.index[3] = buildings.vertices[a + (i * 4)];

		buildings.quads[place] = wall;
		place += 1;
	}

	//back wall
	for (int i = 1; i < floors + 1; i++)
	{
		Quad wall;
		int a = 2;
		int b = 3;
		wall.index[0] = buildings.vertices[a];
		wall.index[1] = buildings.vertices[b];
		wall.index[2] = buildings.vertices[b + (i * 4)];
		wall.index[3] = buildings.vertices[a + (i * 4)];

		buildings.quads[place] = wall;
		place += 1;
	}

	//left wall
	for (int i = 1; i < floors + 1; i++)
	{
		Quad wall;
		int a = 3;
		int b = 0;
		wall.index[0] = buildings.vertices[a];
		wall.index[1] = buildings.vertices[b];
		wall.index[2] = buildings.vertices[b + (i * 4)];
		wall.index[3] = buildings.vertices[a + (i * 4)];

		buildings.quads[place] = wall;
		place += 1;
	}
	
	city[buildingIndex] = buildings;
	for (int i = 0; i <= buildingIndex; i++)
	{
		Building toDraw = city[i];
		drawBuilding(toDraw);
	}
	buildingIndex += 1;
}

void resetCube(void)
{
	//reset cube to original shape and position
	moveX = -7.0;
	moveY = 1.0;
	moveZ = 7.0;

	scaleX = 1.0;
	scaleY = 1.0;
	scaleZ = 1.0;
}

void read(void)
{
	//reading in from the buildings.txt file
	FILE *in;
	in = fopen("buildings.txt", "r");
	int lines = 0;

    while(!feof(in))
	{
		fscanf(in, "%f\n", &insX);
		fscanf(in, "%f\n", &insY);
		fscanf(in, "%f\n", &insZ);
		fscanf(in, "%f\n", &inx);
		fscanf(in, "%f\n", &iny);
		fscanf(in, "%f\n", &inz);
    }
}

void save(float sX, float sY, float sZ, float x, float y, float z)
{
	//apprending to the buildings.txt file
	FILE *out;
	out = fopen("buildings.txt", "a");

	if (out == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(out, "%.1f\n", sX);
	fprintf(out, "%.1f\n", sY);
	fprintf(out, "%.1f\n", sZ);
	fprintf(out, "%.1f\n", x);
	fprintf(out, "%.1f\n", y);
	fprintf(out, "%.1f\n", z);

	fclose(out);
}