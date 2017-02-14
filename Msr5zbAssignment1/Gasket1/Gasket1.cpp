
/* Two-Dimensional Sierpinski Gasket          */
/* Generated Using Randomly Selected Vertices */
/* And Bisection                              */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
static GLfloat spin = 0.0;
void myinit()
{

	/* attributes */

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
	glColor3f(1.0, 0.0, 0.0); /* draw in red */

							  /* set up viewing */
							  /* 500 x 500 window with origin lower left */

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 50.0, 0.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{



	GLfloat vertices[3][2] = { { 0.0,0.0 },{ 25.0,50.0 },{ 50.0,0.0 } }; /* A triangle */

	int j, k;
	//int rand();       /* standard random number generator */
	GLfloat p[2] = { 7.5,5.0 };  /* An arbitrary initial point inside traingle */

	glClear(GL_COLOR_BUFFER_BIT);  /*clear the window */


								   /* compute and plots 5000 new points */
	glPushMatrix();
	glTranslatef(25, 21, 0);
	glRotatef(spin, 0.0, 0.0, 1.0);
	glTranslatef(-25, -21, 0);
	glBegin(GL_POINTS);

	for (k = 0; k<5000; k++)
	{
		j = rand() % 3; /* pick a vertex at random */


						/* Compute point halfway between selected vertex and old point */

		p[0] = (p[0] + vertices[j][0]) / 2.0;
		p[1] = (p[1] + vertices[j][1]) / 2.0;

		/* plot new point */

		glVertex2fv(p);

	}
	glEnd();
	glPopMatrix();
	glFlush(); /* clear buffers */
}


void spinDisplay(void)
{
	spin = spin + 0.5;
	if (spin > 360.0) {
		spin = spin - 360.0;
	}

	glutPostRedisplay();

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
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
void main(int argc, char** argv)
{

	/* Standard GLUT initialization */

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); /* default, not needed */
	glutInitWindowSize(500, 500); /* 500 x 500 pixel window */
	glutInitWindowPosition(0, 0); /* place window top left on display */
	glutCreateWindow("Sierpinski Gasket"); /* window title */
	glutDisplayFunc(display); /* display callback invoked when window opened */
							  //glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	myinit(); /* set attributes */

	glutMainLoop(); /* enter event loop */
}

