#include "OpenGLDefaults.h"
#define PI 3.1415926535898
#define Cos(th) cos(PI/180*(th))
#define Sin(th) sin(PI/180*(th))

/*Michael Rallo msr5zb 12358133*/

double dim = 3;
char* windowName = "Perspective";
int windowWidth = 500;
int windowHeight = 450;

int toggleAxes = 0;
int toggleValues = 1;
int toggleMode = 0;
int th = 0;
int ph = 0;
int fov = 55;
int asp = 1;

GLfloat vertA[3] = { 0.5, 0.5, 0.5 };
GLfloat vertB[3] = {-0.5, 0.5, 0.5 };
GLfloat vertC[3] = {-0.5,-0.5, 0.5 };
GLfloat vertD[3] = { 0.5,-0.5, 0.5 };
GLfloat vertE[3] = { 0.5, 0.5,-0.5 };
GLfloat vertF[3] = {-0.5, 0.5,-0.5 };
GLfloat vertG[3] = {-0.5,-0.5,-0.5 };
GLfloat vertH[3] = { 0.5,-0.5,-0.5 };



void project() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (toggleMode) {
		gluPerspective(fov, asp, dim / 4, 4 * dim);
	}
	else {
		glOrtho(-dim*asp, +dim*asp, -dim, +dim, -dim, +dim);
	}



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*Draws the 3d Axis Grid to the Screen*/
void drawAxis() {
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

void drawShape() {
	glBegin(GL_QUADS);

	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(vertA);
	glVertex3fv(vertB);
	glVertex3fv(vertC);
	glVertex3fv(vertD);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3fv(vertE);
	glVertex3fv(vertF);
	glVertex3fv(vertG);
	glVertex3fv(vertH);
	glEnd();
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	if (toggleMode) {
		double Ex = -2*dim*Sin(th)*Cos(ph);
		double Ey = +2*dim         *Sin(ph);
		double Ez = +2*dim*Cos(th)*Cos(ph);
		printf("Eyes: %f, %f, %f\n", Ex, Ey, Ez);
		gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
	}
	else {


		//Set Inputted Rotation
		glRotated(ph, 1, 0, 0);
		glRotated(th, 0, 1, 0);
		//glRotated(zAngle, 0, 0, 1);
	}



	drawAxis();
	drawShape();

	glFlush();
	glutSwapBuffers();
}




/*Updates Display, Keeping Aspect Ratio if Window is changed*/
void reshape(int width, int height) {
	asp = (height > 0) ? (double)width / height : 1;
	glViewport(0, 0, width, height);
	project();


}

/*Keyboard Input. */
void windowKey(unsigned char key, int x, int y) {
	printf("Key is: %d\n", key);
	if (key == 27) exit(0);
	else if (key == 'a' || key == 'A') toggleAxes = 1 - toggleAxes;
	else if (key == 'v' || key == 'V') toggleValues = 1 - toggleValues;
	else if (key == 'm' || key == 'M') toggleMode = 1 - toggleMode;

	else if (key == '-' && key > 1) fov--;
	else if (key == '+' && key < 179) fov++;


	else if (key == 'D') dim += 0.1;
	else if (key == 'd' && dim > 1) dim -= 0.1;


	project();
	glutPostRedisplay();
}

/*Arrow Key Functionality to Move the Display for the Object*/
void windowSpecial(int key, int x, int y) {
	printf("Special Key is: %d\n", key);
	switch (key) {
	case GLUT_KEY_RIGHT:
		th += 5;
		break;
	case GLUT_KEY_LEFT:
		th -= 5;
		break;
	case GLUT_KEY_UP:
		ph += 5;
		break;
	case GLUT_KEY_DOWN:
		ph -= 5;
		break;
	default:break;
	}

	th %= 360;
	ph %= 360;

	project();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(windowName);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(windowKey);
	glutSpecialFunc(windowSpecial);



	glutMainLoop();
	return 0;

}