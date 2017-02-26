#define WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include<vector>

//Normalize, then use GL_Scale((maxX+minX)/2,(maxX+minX)/2,(maxX+minX)/2)
//Then Translate back to this origin.

/*Globals*/
enum DisplayType { POINT, VECTOR, FACES };
DisplayType displayType = POINT;
std::vector<GLfloat*> vertices;
std::vector<GLint*> faces;
char* path = "../Objs/cube.obj";
char* windowName = "Object Loader";

double dim = 5; /*Dimension of Orthogonal Box*/
double maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

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

	int initFlag = -1;
	while (!feof(file)) {
		fscanf(file, "%c", &c);
		if (c == 'v') {
			arrayfloat = new GLfloat[3];
			fscanf(file, "%f %f %f", &f1, &f2, &f3);
			arrayfloat[0] = f1;
			arrayfloat[1] = f2;
			arrayfloat[2] = f3;
			if (initFlag == -1) {
				minX = f1;
				maxX = f1; 
				minY = f2; 
				maxY = f2; 
				minZ = f3; 
				maxZ = f3; 
				initFlag = 0;
			}

			if (f1 > maxX) { maxX = f1; }
			if (f1 < minX) { minX = f1; }
			if (f2 > maxY) { maxY = f2; }
			if (f2 < minY) { minY = f2; }
			if (f3 > maxZ) { maxZ = f3; }
			if (f3 < minZ) { minZ = f3; }
			printf("Coords: %f %f %f \n", arrayfloat[0], arrayfloat[1], arrayfloat[2]);
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


void init()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	printf("Min Z: %f, Max Z: %f", minZ, maxZ);
	//glOrtho(minX - 5, maxX + 5, minY - 5, maxY + 5, -(minZ-5), -(maxZ+5));
	glOrtho(-5, 5, -5, 5, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();


	
	glPointSize(3.0);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i<vertices.size(); i++)
	{
		printf("Point: %f, %f, %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
		glVertex3fv(vertices[i]);
		//glVertex3f(2.0, 2.0, 2.0);
		//glVertex2i(vertices[i][0], vertices[i][1]);
	}
	glEnd();
	glFlush();
	glutSwapBuffers();


	/*	switch (displayType) {
		case POINT: 
			glBegin(GL_POINTS);
			glColor3f(0.0, 1.0, 0.0);
			for (int i = 0; i<vertices.size(); i++)
			{
				//printf("Point: %f, %f, %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
				glVertex3fv(vertices[i]);
				//glVertex3f(2.0, 2.0, 2.0);
				//glVertex2i(vertices[i][0], vertices[i][1]);
			}
			glEnd();
			break;
		case VECTOR: 
			glBegin(GL_LINES);  // GL_LINE_STRIP
			for (int i = 0; i<faces.size(); i++)
			{
				glVertex3fv(vertices[faces[i][0] - 1]);
				glVertex3fv(vertices[faces[i][1] - 1]);
				glVertex3fv(vertices[faces[i][1] - 1]);
				glVertex3fv(vertices[faces[i][2] - 1]);
				glVertex3fv(vertices[faces[i][2] - 1]);
				glVertex3fv(vertices[faces[i][0] - 1]);
			}
			glEnd();
			break;
		case FACES: 
			glBegin(GL_TRIANGLES);
			for (int i = 0; i<faces.size(); i++)
			{
				glVertex3fv(vertices[faces[i][0] - 1]);
				glVertex3fv(vertices[faces[i][1] - 1]);
				glVertex3fv(vertices[faces[i][2] - 1]);
			}
			glEnd();
			break;
		default:
			break;
	}*/
}


void reshape(int width, int height){

	
	double w2h = (height > 0) ? (double)width / height : 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION_MATRIX);
	glLoadIdentity();
	printf("MinX: %f, MaxX: %f | MinY: %f, MaxY: %f | MinZ: %f, MaxZ: %f \n", minX, maxX, minY, maxY, minZ, maxZ);
	//printf("Otrho: %f, %f, %f, %f, %f, %f \n", -abs(minX*w2h) - 5, +abs(maxX*w2h) + 5, -abs(minY) - 5, +abs(maxY) + 5, abs(minZ) - 5, -abs(maxZ) + 5);
	//glOrtho(-abs(minX*w2h) -5, +abs(maxX*w2h)+5, -abs(minY)-5, +abs(maxY)+5, +abs(minZ)-5, -abs(maxZ)+5);
	printf("Otrho: %f, %f, %f, %f, %f, %f \n", -dim*w2h, +dim*w2h, -dim, +dim, -dim, +dim);
	glOrtho(-dim*w2h, +dim*w2h, -dim, +dim, -dim, +dim);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	glOrtho(minX - 5, maxX + 5, minY - 5, maxY + 5, -(minZ - 5), -(maxZ + 5));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/
}


void keyBoardInput(unsigned char key, int x, int y)
{

	switch (key) {
	case 49:
		printf("LOL");
		loadObject("../Objs/cube.obj");
		break;
	case 50:
		loadObject("../Objs/pig.obj");
		break;
	case 51:
		loadObject("../Objs/teapot.obj");
		break;

	case 97:
		displayType = POINT;
		break;

	case 115:
		displayType = VECTOR;
		break;

	case 100:
		displayType = FACES;
		break;
	default:
		break;
	}
	display();
}





void main(int argc, char *argv[])
{
	loadObject(path);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow(windowName);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyBoardInput);

	glutMainLoop();
}