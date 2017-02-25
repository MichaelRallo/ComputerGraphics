#define WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include<vector>

//Normalize, then use GL_Scale((maxX+minX)/2,(maxX+minX)/2,(maxX+minX)/2)
//Then Translate back to this origin.


std::vector<GLfloat*> vertices;
std::vector<GLint*> faces;

static GLfloat spin = 0.0;

enum DisplayType { POINT, VECTOR, FACES };
DisplayType displayType = POINT;
char* path = "../Objs/cube.obj";


void loadObject(char* path) {

	FILE *file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
	}
	printf("Grabbed file successfully!\n");

	char c;
	GLfloat f1, f2, f3, *arrayfloat;
	GLint d1, d2, d3, *arrayint;
	vertices.clear();
	faces.clear();

	while (!feof(file)) {
		fscanf(file, "%c", &c);
		if (c == 'v') {
			arrayfloat = new GLfloat[3];
			fscanf(file, "%f %f %f", &f1, &f2, &f3);
			arrayfloat[0] = f1;
			arrayfloat[1] = f2;
			arrayfloat[2] = f3;
			vertices.push_back(arrayfloat);
		}
		else if (c == 'f') {
			arrayint = new GLint[3];
			fscanf(file, "%d %d %d", &d1, &d2, &d3);
			arrayint[0] = d1;
			arrayint[1] = d2;
			arrayint[2] = d3;
			faces.push_back(arrayint);
		}
	}
	fclose(file);
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotatef(spin, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(3.2);
	glBegin(GL_POINTS);
	for (int i = 0; i<vertices.size(); i++)
	{
		glVertex3fv(vertices[i]);
		printf("Point: %f %f %f", vertices[i][0]);
	}
	glEnd();

	glPopMatrix();

	glutSwapBuffers();
}

void spinDisplay(void)
{
	spin = spin + 2.0;
	if (spin > 360.0)
		spin = spin - 360.0;
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(spinDisplay);
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(NULL);
		break;
	default:
		break;
	}
}

/*
*  Request double buffer display mode.
*  Register mouse input callback functions
*/
int main(int argc, char** argv)
{
	loadObject("../Objs/cube.obj");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;   /* ANSI C requires main to return int. */
}