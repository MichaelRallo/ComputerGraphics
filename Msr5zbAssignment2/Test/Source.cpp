#include "OpenGLDefaults.h"

/*Env Globals*/
double dim = 4;
char* windowName = "Test";
int windowWidth = 600;
int windowHeight = 600;

/*State Globals*/
enum Axis { AXIS_ON, ASIX_OFF };
Axis axis = AXIS_ON;

enum ViewMode {VIEW_PROJECTION, VIEW_ORTHOGONAL};
ViewMode viewMode = VIEW_ORTHOGONAL;
int aziViewAngle = 0;
int eleViewAngle = 0;
int fieldOfView = 55;
double aspectRatio = 1;
double scaler = 1;

/*Project Globals*/
std::vector<GLfloat*> vertices;
std::vector<GLint*> faces;
double maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

/*Display Types*/
enum DisplayType { POINT, VECTOR, FACES };
DisplayType displayType = VECTOR;




void drawAxis() {
	if (axis == AXIS_ON) {
		double axisLength = 3;
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3d(0, 0, 0);
		glVertex3d(axisLength, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, axisLength, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, axisLength);
		glEnd();


	}
}


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


void scale() {
	/*Scale*/
	double distanceX = abs(maxX - minX);
	double distanceY = abs(maxY - minY);
	double distanceZ = abs(maxZ - minZ);
	printf("D: %f %f %f\n", distanceX, distanceY, distanceZ);

	double superMax;
	if (distanceX > distanceY && distanceX > distanceZ) {
		superMax = distanceX;
	}
	else if (distanceY > distanceX && distanceY > distanceZ) {
		superMax = distanceY;
	}
	else {
		superMax = distanceZ;
	}
	printf("Max Distance is: %f\n", superMax);
	scaler = (dim - 0.5) / superMax;

	glScaled(scaler, scaler, scaler);
}

void display() {



	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glRotated(90, 0, 0, 1);
	glRotated(70, 0, 1, 0);
	glRotated(200, 0, 0, 1);
	glRotated(aziViewAngle, 0, 1, 0);
	glRotated(eleViewAngle, 0, 0, 1);
	drawAxis();

	glColor3f(0.0, 1.0, 0.0);
	glPointSize(4);

	printf("Azi: %d, Ele: %d\n", aziViewAngle, eleViewAngle);
	printf("MinX: %f, MaxX: %f | MinY: %f, MaxY: %f | MinZ: %f, MaxZ: %f\n", minX, maxX, minY, maxY, minZ, maxZ);
	printf("Middle X: %f, Middle Y: %f, Middle Z: %f\n", (minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2);
	scale();
	glTranslated(-(minX + maxX) / 2, -(minY + maxY) / 2, -(minZ + maxZ) / 2);
	
	

	

	switch (displayType) {
	case POINT:
		glBegin(GL_POINTS);
		for (int i = 0; i < vertices.size(); i++)
		{
			glVertex3fv(vertices[i]);
		}
		glEnd();
		break;
	case VECTOR:
		glBegin(GL_LINES);  // GL_LINE_STRIP
		for (int i = 0; i < faces.size(); i++)
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
		for (int i = 0; i < faces.size(); i++)
		{
			glVertex3fv(vertices[faces[i][0] - 1]);
			glVertex3fv(vertices[faces[i][1] - 1]);
			glVertex3fv(vertices[faces[i][2] - 1]);
		}
		glEnd();
		break;
	default:
		break;
	}

	glFlush();
	glutSwapBuffers();
}


void reshape(int width, int height) {
	aspectRatio = (height > 0) ? (double)width / height : 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	printf("Ratio: %f %f | %f %f | %f %f\n", -dim*aspectRatio, +dim*aspectRatio, -dim, +dim, -dim, +dim);
	glOrtho(-dim*aspectRatio, +dim*aspectRatio, -dim, +dim, -dim, +dim);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void windowKey(unsigned char key, int x, int y) {

	if (key == 27) exit(0);
	switch (key) {
	case 27:
		exit(0);
		break;
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
	glutPostRedisplay();
}

void windowSpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		eleViewAngle += 5;
		break;
	case GLUT_KEY_RIGHT:
		eleViewAngle -= 5;
		break;
	case GLUT_KEY_UP:
		aziViewAngle += 5;
		break;
	case GLUT_KEY_DOWN:
		aziViewAngle -= 5;
		break;
	default:break;
	}

	/*Keep angles +/- 360 degrees*/
	aziViewAngle %= 360;
	eleViewAngle %= 360;
	glutPostRedisplay();
}
int main(int argc, char* argv[]) {

	loadObject("../Objs/cube.obj");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(windowName);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(windowKey);
	glutSpecialFunc(windowSpecial);

	glutMainLoop();
	return 0;

}