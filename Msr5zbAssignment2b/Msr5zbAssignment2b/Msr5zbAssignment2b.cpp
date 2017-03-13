	#include "OpenGLDefaults.h"
	#define PI 3.1415926535898
	#define Cos(yAngle) cos(PI/180*(yAngle))
	#define Sin(yAngle) sin(PI/180*(yAngle))

	/*Michael Rallo msr5zb 12358133*/

	/*Env Globals*/
	double dim = 4;
	char* windowName = "Perspective";
	int windowWidth = 600;
	int windowHeight = 600;


	/*State Globals*/
	enum Axis { AXIS_ON, AXIS_OFF };
	Axis axis = AXIS_ON;
	double xAngle = 0;
	double yAngle = 0;
	double zAngle = 0;
	double aspectRatio = 1;
	double scaler = 1;
	double xTranslate = 0;
	double yTranslate = 0;
	double zTranslate = 0;

	int xMouse;
	int yMouse;
	int oldX;
	int oldY;
	int zIndexEnabled = 0;

	bool isPressed = false;

	int dragging = 0;
	enum ViewMode {VIEW_PROJECTION, VIEW_ORTHOGONAL};
	ViewMode viewMode = VIEW_PROJECTION;
	int fieldOfView = 55;

	/*Project Globals*/
	std::vector<GLfloat*> vertices;
	std::vector<GLint*> faces;
	double maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

	/*Display Types*/
	enum DisplayType { POINT, VECTOR, FACES };
	DisplayType displayType = VECTOR;

	/*Draws the 3d Axis Grid to the Screen*/
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


	/*Scales the object that has been Loaded in*/
	void scale() {
		/*Scale*/
		double distanceX = abs(maxX - minX);
		double distanceY = abs(maxY - minY);
		double distanceZ = abs(maxZ - minZ);

		//Find the max distance in order to find best scaler.
		double maxDistance;
		if (distanceX > distanceY && distanceX > distanceZ) {
			maxDistance = distanceX;
		}
		else if (distanceY > distanceX && distanceY > distanceZ) {
			maxDistance = distanceY;
		}
		else {
			maxDistance = distanceZ;
		}
		//Calculate Scaler
		scaler = (dim - 0.5) / maxDistance;
	}


	/*Loads the .OBJ file given*/
	void loadObject(char* path) {

		//Grab File
		FILE *file = fopen(path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}
		printf("Grabbed file successfully!\n");

		//Data Holders
		char c;
		GLfloat f1, f2, f3, *arrayfloat;
		GLint d1, d2, d3, *arrayint;
		vertices.clear();
		faces.clear();

		int initFlag = -1;
		while (!feof(file)) {
			//Get Datas
			fscanf(file, "%c", &c);

			//Store Vertices
			if (c == 'v') {
				arrayfloat = new GLfloat[3];
				fscanf(file, "%f %f %f", &f1, &f2, &f3);
				arrayfloat[0] = f1;
				arrayfloat[1] = f2;
				arrayfloat[2] = f3;
				vertices.push_back(arrayfloat);

				//Set Mins and Maxes, will be used for Scaling and Translating.
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
			}

			//Store Faces
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

		//Scale the Object
		scale();
	}


	void display() {

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glLoadIdentity();

		//Adjust According to View Mode
		if (viewMode == VIEW_PROJECTION) {
			double Ex = -2*dim*Sin(yAngle)*Cos(xAngle);
			double Ey = +2*dim            *Sin(xAngle);
			double Ez = +2*dim*Cos(yAngle)*Cos(xAngle);
			//printf("EYE: %f, %f, %f\n", Ex, Ey, Ez);
			printf("Up: %f, Z:%f\n", Cos(xAngle), Sin(zAngle));
			gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(xAngle), 0);
			//gluLookAt(xAngle, yAngle, zAngle, 0, 0, 0, 0, Cos(xAngle), 0);


		}
		else {
			//Angled Rotation for Better View of 3d Objects
			printf("Up: %f, Z:%f\n", yAngle, zAngle);

			//Set Inputted Rotation
			glRotated(xAngle, 1, 0, 0);
			glRotated(yAngle, 0, 1, 0);
			if (zIndexEnabled) {
				glRotated(zAngle, 0, 0, 1);
			}
		}

		//Draw Axis
		drawAxis();

		//Colors/Sizes
		glColor3f(0.0, 1.0, 0.0);
		glPointSize(4);

		//Scale
		glScaled(scaler, scaler, scaler);

		//Move Object to Center
		glTranslated(xTranslate, yTranslate, zTranslate);
		glTranslated(-(minX + maxX) / 2, -(minY + maxY) / 2, -(minZ + maxZ) / 2);

		//Display with Desired Type
		switch (displayType) {
		case POINT:
			glBegin(GL_POINTS);
			for (int i = 0; i < vertices.size(); i++) {
				glVertex3fv(vertices[i]);
			}
			glEnd();
			break;

		case VECTOR:
			glBegin(GL_LINES);
			for (int i = 0; i < faces.size(); i++) {
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
			for (int i = 0; i < faces.size(); i++) {
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


	/*Display Details*/
	void project() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Adjust according to ViewMode Active
		if (viewMode == VIEW_PROJECTION) {
			gluPerspective(fieldOfView, aspectRatio, dim / 4, 4 * dim);
		}
		else {
			glOrtho
			(-dim*aspectRatio, +dim*aspectRatio, -dim, +dim, -dim, +dim);
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	/*Updates Display, Keeping Aspect Ratio if Window is changed*/
	void reshape(int width, int height) {
		aspectRatio = (height > 0) ? (double)width / height : 1;
		glViewport(0, 0, width, height);
		project();
	}

	/*Keyboard Input. */
	void windowKey(unsigned char key, int x, int y) {

		if (key == 27) exit(0);
		switch (key) {

		//Easy Exit
		case 27: //ESC
			exit(0);
			break;

		//Object Loaders
		case '1':
			loadObject("../Objs/cube.obj");
			break;
		case '2': 
			loadObject("../Objs/pig.obj");
			break;
		case '3':
			loadObject("../Objs/teapot.obj");
			break;

		//Display Types
		case 'a': 
			displayType = POINT;
			break;
		case 's':
			displayType = VECTOR;
			break;
		case 'd':
			displayType = FACES;
			break;

		//Toggle Axis
		case 'i':
			if (axis == AXIS_ON) { axis = AXIS_OFF; }
			else { axis = AXIS_ON; }
			break;

		//Toggle Perspective
		case 'p':
			if (viewMode == VIEW_PROJECTION) { viewMode = VIEW_ORTHOGONAL; }
			else { viewMode = VIEW_PROJECTION; }
			break;	

		//Scale
		case 'm':
			scaler += 0.01;
			break;

		case 'n':
			scaler -= 0.01;
			break;

		//Zooms
		case 43://+
			if (viewMode == VIEW_PROJECTION) { fieldOfView--; }
			else { dim -= 0.05; }
			break;

		case 45://-
			if (viewMode == VIEW_PROJECTION) { fieldOfView++; }
			else { dim += 0.05; }
			break;


		//Translations
		case 'x':
			xTranslate += 0.1;
			break;
		case 'X':
			xTranslate -= 0.1;
			break;
		case 'y':
			yTranslate += 0.1;
			break;
		case 'Y':
			yTranslate -= 0.1;
			break;
		case 'z':
			zTranslate += 0.1;
			break;
		case 'Z':
			zTranslate -= 0.1;
			break;

		//Enable 360 Degree Rotation for Ortho View
		case 'o':
			if (zIndexEnabled) { zIndexEnabled = 0; }
			else { zIndexEnabled = 1; }
			break;

		//Reset
		case 'r':
			xAngle = 0;
			yAngle = 0;
			zAngle = 0;
			xTranslate = 0;
			yTranslate = 0;
			zTranslate = 0;
			scale();
			break;

		default:
			break;
		}
		project();
		glutPostRedisplay();
	}

	/*Arrow Key Functionality to Move the Display for the Object*/
	/*Used for Precise Rotation!*/
	void windowSpecial(int key, int x, int y) {
		printf("Special Key is: %d\n", key);
		switch (key) {
		case GLUT_KEY_RIGHT:
			yAngle += 5;
			break;
		case GLUT_KEY_LEFT:
			yAngle -= 5;
			break;
		case GLUT_KEY_UP:
			xAngle += 5;
			break;
		case GLUT_KEY_DOWN:
			xAngle -= 5;
			break;
		default:break;
		}

		xAngle = fmod(xAngle, 360);
		yAngle = fmod(yAngle, 360);

		project();
		glutPostRedisplay();
	}

	/*Mouse Actions*/
	void mouseActions(int button, int state, int x, int y) {

		switch (button) {
		case GLUT_LEFT_BUTTON:
			//Keep track of Coords when Rotating
			if (state == GLUT_DOWN){
				oldX = x;
				oldY = y;
				isPressed = true;
			}
			break;
		default:
			isPressed = false;
			break;
	}

		project();
		glutPostRedisplay();
	}

	/*Mouse Movement*/
	void myMotion(int x, int y) {
		if (isPressed)
		{
			//Rotate Algs, Note Z is only used for Ortho Z-Enabled View
			yAngle += 0.01*(x - oldX);
			xAngle += 0.01*(y - oldY);
			zAngle += 0.001*((x - oldX)*(y - oldY));

			xAngle = fmod(xAngle, 360);
			yAngle = fmod(yAngle, 360);
			zAngle = fmod(zAngle, 360);
		}
		project();
		glutPostRedisplay();
	}

	int main(int argc, char* argv[]) {

		//Setups
		loadObject("../Objs/cube.obj");
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(windowWidth, windowHeight);
		glutCreateWindow(windowName);

		//Inputs
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(windowKey);
		glutSpecialFunc(windowSpecial);
		glutMouseFunc(mouseActions);
		glutMotionFunc(myMotion);

		glutMainLoop();
		return 0;

	}