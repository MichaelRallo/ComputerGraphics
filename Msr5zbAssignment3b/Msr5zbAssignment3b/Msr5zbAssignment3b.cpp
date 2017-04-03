#define STB_IMAGE_IMPLEMENTATION
#include "OpenGLDefaults.h"
#include "stb_image.h"
#define PI 3.1415926535898
#define Cos(yAngle) cos(PI/180*(yAngle))
#define Sin(yAngle) sin(PI/180*(yAngle))

/*Michael Rallo msr5zb 12358133*/

/*Env Globals*/
double dim = 4;
char* windowName = "Lighting";
int windowWidth = 600;
int windowHeight = 600;

/*State Globals*/
enum Axis { AXIS_ON, AXIS_OFF };
Axis axis = AXIS_ON;
enum AspectRatioType { AUTO, CUSTOM };
AspectRatioType aspectRatioType = AUTO;
double aspectRatio = 1;
double scaler = 1;

/*View and Positioning*/
int fieldOfView = 70;
double near = 4;
double far = 4;
double xAngle = 0;
double yAngle = 0;
double zAngle = 0;
double xTranslate = 0;
double yTranslate = 0;
double zTranslate = 0;

/*Dragging*/
int xMouse;
int yMouse;
int oldX;
int oldY;
int dragging = 0;
bool isPressed = false;

/*Light Stuff*/
enum LightMode { ON, OFF };
LightMode light0Mode = ON;
LightMode light1Mode = ON;
double lightDistance = 5;
float lightY = 0;
double lightX = 90;
double light2X = 45;
double lightAmbient = 35;
double lightDiffuse = 100;
double lightSpecular = 0;
double lightEmission = 0;
float lightShininess = 0;
float globalAmbientLight = 15;
float shinyVector[1];
double materialAmbient = 35;
double materialDiffuse = 100;
double materialSpecular = 0;
float white[] = { 1.0, 1.0, 1.0, 1.0 };
float red[] = { 1.0, 0.0, 0.0, 1.0 };
float green[] = { 0.0, 1.0, 0.0, 1.0 };
float blue[] = { 0.0, 0.0, 1.0, 1.0 };
float yellow[] = { 1.0, 1.0, 0.0, 1.0 };
float purple[] = { 1.0, 0.0, 1.0, 1.0 };
float cyan[] = { 0.0, 1.0, 1.0, 1.0 };

/*Project Globals*/
std::vector<GLfloat*> vertices;
std::vector<GLfloat*> fnormals;
std::vector<GLfloat*> vnormals;
std::vector<GLint*> faces;
double maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

/*Display Types*/
enum DisplayType { POINT, VECTOR, FACES };
DisplayType displayType = FACES;

enum ShadeType { FLAT, SMOOTH };
ShadeType shadeType = SMOOTH;

/*Textures*/
const char* textures[2];
int currentTexture;


int loadTexture() {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data;
	image_data = stbi_load(textures[currentTexture], &x, &y, &n, force_channels);
	if (image_data) {
		static GLuint texName;
		glEnable(GL_TEXTURE_2D);

		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		free(image_data);
		return texName;
	}
	else {
		printf("Failed &n\n");
		return 0;
	}

}
void initializeTextures() {

	textures[0] = "../Textures/checker.jpg";
	textures[1] = "../Textures/mandrill.jpg";
	currentTexture = 1;
}


/*Draws the 3d Axis Grid to the Screen*/
void drawAxis() {
	if (axis == AXIS_ON) {
		glDisable(GL_LIGHTING);
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
		if (light0Mode == ON || light1Mode == ON) {
			glEnable(GL_LIGHTING);
		}
	}
}

//Vertex Helper function for Spheres
void vertex(double th, double ph) {
	double x = Sin(th)*Cos(ph);
	double y = Cos(th)*Cos(ph);
	double z = Sin(ph);
	glNormal3d(x, y, z);
	glVertex3d(x, y, z);
}

//Spheres to Represent Lights
void sphere(double x, double y, double z, double r, double rot) {
	int th, ph;
	float yellow[] = { 1.0, 1.0, 0.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SHININESS, shinyVector);
	glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);

	glPushMatrix();

	glTranslated(x, y, z);
	glScaled(r, r, r);
	glRotated(rot, 0, 1, 0);

	for (ph = -90; ph < 90; ph += 5) {
		glBegin(GL_QUAD_STRIP);
		for (th = 0; th <= 360; th += 2 * 5) {
			vertex(th, ph);
			vertex(th, ph + 5);
		}
		glEnd();
	}
	glPopMatrix();
}

void drawLight() {
	if (light0Mode == ON || light1Mode == ON) {

		//Light Properties
		float Ambient0[] = { 0.01*lightAmbient, 0.0, 0.0, 1.0 };
		float Diffuse0[] = { 0.01*lightDiffuse, 0.0, 0.0, 1.0 };
		float Specular0[] = { 0.1*lightSpecular, 0.0, 0.0, 1.0 };
		float Position0[] = { (lightDistance*Sin(lightX)) / (scaler*1.7), (lightY) / (scaler*1.7), (lightDistance*Cos(lightX)) / (scaler*1.7), 1.0 / (scaler * 4) };

		float Ambient1[] = { 0.0, 0.01*lightAmbient, 0.01, 1.0 };
		float Diffuse1[] = { 0.0, 0.01*lightDiffuse, 0.0, 1.0 };
		float Specular1[] = { 0.0, 0.1*lightSpecular, 0.0, 1.0 };
		float Position1[] = { -(lightDistance*Sin(lightX)) / (scaler*1.7), (lightY) / (scaler*1.7), (lightDistance*Cos(lightX)) / (scaler*1.7), 1.0 / (scaler * 4) };

		//Light 1
		if (light0Mode == ON) {
			glDisable(GL_LIGHTING);
			glColor3fv(red);
			sphere(Position0[0], Position0[1], Position0[2], Position0[3], 0);
			glEnable(GL_LIGHTING);

			glEnable(GL_LIGHT0);
			glLightfv(GL_LIGHT0, GL_SPECULAR, Specular0);
			glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse0);
			glLightfv(GL_LIGHT0, GL_POSITION, Position0);
		}
		else {
			glDisable(GL_LIGHT0);
		}

		//Light 2
		if (light1Mode == ON) {
			glDisable(GL_LIGHTING);
			glColor3fv(green);
			sphere(Position1[0], Position1[1], Position1[2], Position1[3], 0);
			glEnable(GL_LIGHTING);

			glEnable(GL_LIGHT1);
			glLightfv(GL_LIGHT1, GL_SPECULAR, Specular1);
			glLightfv(GL_LIGHT1, GL_AMBIENT, Ambient1);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, Diffuse1);
			glLightfv(GL_LIGHT1, GL_POSITION, Position1);
		}
		else {
			glDisable(GL_LIGHT1);
		}

	}
	else {
		glDisable(GL_LIGHTING);
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

void normalize() {
	vnormals.clear();
	std::vector<GLfloat*> uVector, vVector;
	GLfloat *uArray, *vArray, *nArray, *nvArray;

	//Calculate Vertex Normals!
	for (GLfloat vertex = 0; vertex < vertices.size(); vertex++) {

		//Calculate Face normals of all Faces that includes our vertex
		fnormals.clear();
		for (int i = 0; i < faces.size(); i++) {

			if ((faces[i][0] - 1) == vertex || (faces[i][1] - 1) == vertex || (faces[i][2] - 1) == vertex) {
				//Cauclate Normal Vector
				uArray = new GLfloat[3];
				uArray[0] = vertices[faces[i][1] - 1][0] - vertices[faces[i][0] - 1][0];
				uArray[1] = vertices[faces[i][1] - 1][1] - vertices[faces[i][0] - 1][1];
				uArray[2] = vertices[faces[i][1] - 1][2] - vertices[faces[i][0] - 1][2];
				uVector.push_back(uArray);

				vArray = new GLfloat[3];
				vArray[0] = vertices[faces[i][2] - 1][0] - vertices[faces[i][0] - 1][0];
				vArray[1] = vertices[faces[i][2] - 1][1] - vertices[faces[i][0] - 1][1];
				vArray[2] = vertices[faces[i][2] - 1][2] - vertices[faces[i][0] - 1][2];
				vVector.push_back(vArray);

				nArray = new GLfloat[3];
				nArray[0] = (uArray[1] * vArray[2]) - (uArray[2] * vArray[1]);
				nArray[1] = (uArray[2] * vArray[0]) - (uArray[0] * vArray[2]);
				nArray[2] = (uArray[0] * vArray[1]) - (uArray[1] * vArray[0]);

				//Divide Normal by Magnitude
				double mag = sqrt((nArray[0] * nArray[0]) + (nArray[1] * nArray[1]) + (nArray[2] * nArray[2]));
				nArray[0] = nArray[0] / mag;
				nArray[1] = nArray[1] / mag;
				nArray[2] = nArray[2] / mag;

				fnormals.push_back(nArray);
			}
		}
		//Average by Adding and Dividing by Magnitude
		nvArray = new GLfloat[3];
		nvArray[0] = 0;
		nvArray[1] = 0;
		nvArray[2] = 0;
		for (int f = 0; f < fnormals.size(); f++) {
			nvArray[0] += fnormals[f][0];
			nvArray[1] += fnormals[f][1];
			nvArray[2] += fnormals[f][2];
		}
		double mag = sqrt((nvArray[0] * nvArray[0]) + (nvArray[1] * nvArray[1]) + (nvArray[2] * nvArray[2]));
		nvArray[0] = nvArray[0] / mag;
		nvArray[1] = nvArray[1] / mag;
		nvArray[2] = nvArray[2] / mag;
		vnormals.push_back(nvArray);
	}
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
			//Set Vertices
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
	normalize();
	scale();
}


void handleTexCoord(int faceIndex, GLfloat* workingVertex){
	//Given Three Vertexes, find which position it is in (Bottom Left, Bottom Right, Top Left, Top Right)
	//Also note images are loaded in upside-down
	GLfloat* v0 = vertices[faces[faceIndex][0] - 1];
	GLfloat* v1 = vertices[faces[faceIndex][1] - 1];
	GLfloat* v2 = vertices[faces[faceIndex][2] - 1];

	if(workingVertex == v0) {
		//XY Plane
		if (v0[2] == v1[2] && v0[2] == v2[2]){
			//Top Right
			if((v0[1] - v1[1] > 0 || v0[1] - v2[1] > 0) && (v0[0] - v1[0] > 0 || v0[0] - v2[0] > 0)){glTexCoord2f(0, 0);}
			//Top Left
			if((v0[1] - v1[1] > 0 || v0[1] - v2[1] > 0) && (v0[0] - v1[0] < 0 || v0[0] - v2[0] < 0)){glTexCoord2f(1, 0);}
			//Bottom Right
			if((v0[1] - v1[1] < 0 || v0[1] - v2[1] < 0) && (v0[0] - v1[0] > 0 || v0[0] - v2[0] > 0)){glTexCoord2f(0, 1);}
			//Bottom Left
			if((v0[1] - v1[1] < 0 || v0[1] - v2[1] < 0) && (v0[0] - v1[0] < 0 || v0[0] - v2[0] < 0)){glTexCoord2f(1, 1);}
		}

		//XZ Plane
		else if (v0[1] == v1[1] && v0[1] == v2[1]) {
			//Top Right
			if ((v0[0] - v1[0] > 0 || v0[0] - v2[0] > 0) && (v0[2] - v1[2] > 0 || v0[2] - v2[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v0[0] - v1[0] > 0 || v0[0] - v2[0] > 0) && (v0[2] - v1[2] < 0 || v0[2] - v2[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v0[0] - v1[0] < 0 || v0[0] - v2[0] < 0) && (v0[2] - v1[2] > 0 || v0[2] - v2[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v0[0] - v1[0] < 0 || v0[0] - v2[0] < 0) && (v0[2] - v1[2] < 0 || v0[2] - v2[2] < 0)) { glTexCoord2f(1, 1); }
		}

		//YZ Plane
		else if (v0[0] == v1[0] && v0[0] == v2[0]) {
			//Top Right
			if ((v0[1] - v1[1] > 0 || v0[1] - v2[1] > 0) && (v0[2] - v1[2] > 0 || v0[2] - v2[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v0[1] - v1[1] > 0 || v0[1] - v2[1] > 0) && (v0[2] - v1[2] < 0 || v0[2] - v2[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v0[1] - v1[1] < 0 || v0[1] - v2[1] < 0) && (v0[2] - v1[2] > 0 || v0[2] - v2[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v0[1] - v1[1] < 0 || v0[1] - v2[1] < 0) && (v0[2] - v1[2] < 0 || v0[2] - v2[2] < 0)) { glTexCoord2f(1, 1); }
		}

		//Multi-Planar
		else { glTexCoord2f(0, 0); }

	}
	else if (workingVertex == v1) {
		//XY Plane
		if (v1[2] == v0[2] && v1[2] == v2[2]) {
			//Top Right
			if ((v1[1] - v0[1] > 0 || v1[1] - v2[1] > 0) && (v1[0] - v0[0] > 0 || v1[0] - v2[0] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v1[1] - v0[1] > 0 || v1[1] - v2[1] > 0) && (v1[0] - v0[0] < 0 || v1[0] - v2[0] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v1[1] - v0[1] < 0 || v1[1] - v2[1] < 0) && (v1[0] - v0[0] > 0 || v1[0] - v2[0] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v1[1] - v0[1] < 0 || v1[1] - v2[1] < 0) && (v1[0] - v0[0] < 0 || v1[0] - v2[0] < 0)) { glTexCoord2f(1, 1); }
		}

		//XZ Plane
		else if (v1[1] == v0[1] && v1[1] == v2[1]) {
			//Top Right
			if ((v1[0] - v0[0] > 0 || v1[0] - v2[0] > 0) && (v1[2] - v0[2] > 0 || v1[2] - v2[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v1[0] - v0[0] > 0 || v1[0] - v2[0] > 0) && (v1[2] - v0[2] < 0 || v1[2] - v2[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v1[0] - v0[0] < 0 || v1[0] - v2[0] < 0) && (v1[2] - v0[2] > 0 || v1[2] - v2[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v1[0] - v0[0] < 0 || v1[0] - v2[0] < 0) && (v1[2] - v0[2] < 0 || v1[2] - v2[2] < 0)) { glTexCoord2f(1, 1); }
		}

		//YZ Plane
		else if (v1[0] == v0[0] && v1[0] == v2[0]) {
			//Top Right
			if ((v1[1] - v0[1] > 0 || v1[1] - v2[1] > 0) && (v1[2] - v0[2] > 0 || v1[2] - v2[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v1[1] - v0[1] > 0 || v1[1] - v2[1] > 0) && (v1[2] - v0[2] < 0 || v1[2] - v2[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v1[1] - v0[1] < 0 || v1[1] - v2[1] < 0) && (v1[2] - v0[2] > 0 || v1[2] - v2[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v1[1] - v0[1] < 0 || v1[1] - v2[1] < 0) && (v1[2] - v0[2] < 0 || v1[2] - v2[2] < 0)) { glTexCoord2f(1, 1); }
		}
		//Multi-Planar
		else { glTexCoord2f(0, 1); }
	}
	else if(workingVertex == v2) {
		//XY Plane
		//printf("ZXXXXXX\n");
		if (v2[2] == v0[2] && v2[2] == v1[2]) {
			//Top Right
			if ((v2[1] - v0[1] > 0 || v2[1] - v1[1] > 0) && (v2[0] - v0[0] > 0 || v2[0] - v1[0] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v2[1] - v0[1] > 0 || v2[1] - v1[1] > 0) && (v2[0] - v0[0] < 0 || v2[0] - v1[0] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v2[1] - v0[1] < 0 || v2[1] - v1[1] < 0) && (v2[0] - v0[0] > 0 || v2[0] - v1[0] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v2[1] - v0[1] < 0 || v2[1] - v1[1] < 0) && (v2[0] - v0[0] < 0 || v2[0] - v1[0] < 0)) { glTexCoord2f(1, 1); }
		}

		//XZ Plane
		else if (v2[1] == v0[1] && v2[1] == v1[1]) {
			//Top Right
			if ((v2[0] - v0[0] > 0 || v2[0] - v1[0] > 0) && (v2[2] - v0[2] > 0 || v2[2] - v1[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v2[0] - v0[0] > 0 || v2[0] - v1[0] > 0) && (v2[2] - v0[2] < 0 || v2[2] - v1[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v2[0] - v0[0] < 0 || v2[0] - v1[0] < 0) && (v2[2] - v0[2] > 0 || v2[2] - v1[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v2[0] - v0[0] < 0 || v2[0] - v1[0] < 0) && (v2[2] - v0[2] < 0 || v2[2] - v1[2] < 0)) { glTexCoord2f(1, 1); }
		}

		//YZ Plane
		else if (v2[0] == v0[0] && v2[0] == v1[0]) {
			//Top Right
			if ((v2[1] - v0[1] > 0 || v2[1] - v1[1] > 0) && (v2[2] - v0[2] > 0 || v2[2] - v1[2] > 0)) { glTexCoord2f(0, 0); }
			//Top Left
			if ((v2[1] - v0[1] > 0 || v2[1] - v1[1] > 0) && (v2[2] - v0[2] < 0 || v2[2] - v1[2] < 0)) { glTexCoord2f(1, 0); }
			//Bottom Right
			if ((v2[1] - v0[1] < 0 || v2[1] - v1[1] < 0) && (v2[2] - v0[2] > 0 || v2[2] - v1[2] > 0)) { glTexCoord2f(0, 1); }
			//Bottom Left
			if ((v2[1] - v0[1] < 0 || v2[1] - v1[1] < 0) && (v2[2] - v0[2] < 0 || v2[2] - v1[2] < 0)) { glTexCoord2f(1, 1); }
		}
		//Multi-Planar
		else { glTexCoord2f(1, 1); }
	}

}

void display() {

	//Display Variables
	float globalAmbientLightArray[4] = { 0.01*globalAmbientLight, 0.01*globalAmbientLight, 0.01*globalAmbientLight, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLightArray);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	if (shadeType == SMOOTH) { glShadeModel(GL_SMOOTH); }
	else { glShadeModel(GL_FLAT); }

	glLoadIdentity();

	//Perspective/Camera View
	double Ex = -2 * dim*Sin(yAngle)*Cos(xAngle);
	double Ey = +2 * dim            *Sin(xAngle);
	double Ez = +2 * dim*Cos(yAngle)*Cos(xAngle);
	gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(xAngle), 0);


	//Draw Axis
	drawAxis();
	glPointSize(4);

	//Scale
	glScaled(scaler, scaler, scaler);

	//Light
	drawLight();

	//Move Object to Center
	glTranslated(xTranslate, yTranslate, zTranslate);
	glTranslated(-(minX + maxX) / 2, -(minY + maxY) / 2, -(minZ + maxZ) / 2);

	//Material Stuffs
	glColor3f(1.0, 1.0, 1.0);
	//GLfloat materialAmbientArray[] = { 0.1*materialAmbient, 0.1*materialAmbient, 0.1*materialAmbient, 1.0 };
	//GLfloat materialDiffuseArray[] = { 0.1*materialDiffuse, 0.1*materialDiffuse, 0.1*materialDiffuse, 1.0 };
	//GLfloat materialSpecularArray[] = { 0.1*materialSpecular, 0.1*materialSpecular, 0.1*materialSpecular, 1.0 };
	GLfloat materialAmbientArray[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat materialDiffuseArray[] = { 1.0, 0.8, 0.0, 1.0 };
	GLfloat materialSpecularArray[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat shine = 10.0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbientArray);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuseArray);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecularArray);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);

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
		loadTexture();
		glEnable(GL_NORMALIZE);
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < faces.size(); i++) {

			glNormal3fv(vnormals[faces[i][0] - 1]); 
			handleTexCoord(i, vertices[faces[i][0] - 1]);
			glVertex3fv(vertices[faces[i][0] - 1]);

			glNormal3fv(vnormals[faces[i][1] - 1]);
			handleTexCoord(i, vertices[faces[i][1] - 1]);
			glVertex3fv(vertices[faces[i][1] - 1]);

			glNormal3fv(vnormals[faces[i][2] - 1]);
			handleTexCoord(i, vertices[faces[i][2] - 1]);
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
	gluPerspective(fieldOfView, aspectRatio, dim / near, far * dim);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*Updates Display, Keeping Aspect Ratio if Window is changed*/
void reshape(int width, int height) {

	if (aspectRatioType == AUTO) {
		aspectRatio = (height > 0) ? (double)width / height : 1;
	}
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
	case '4':
		if (currentTexture == 0) { currentTexture = 1; }
		else { currentTexture = 0; }
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

		//Scale
	case 'b':
		scaler += 0.01;
		break;

	case 'B':
		scaler -= 0.01;
		break;

		//Field of Vision Zoom in/out
	case 43://+
		fieldOfView--;
		break;

	case 45://-
		fieldOfView++;
		break;


		//Aspect Ratio
	case 't':
		aspectRatio += 0.1;
		break;
	case 'T':
		aspectRatio -= 0.1;
		break;


		//Near and Far Clipping Plane
	case 'g':
		near -= 0.1;
		break;
	case 'G':
		near += 0.1;
		break;
	case 'f':
		far -= 0.1;
		break;
	case 'F':
		far += 0.1;
		break;

		//Light
	case 'l':
		if (light0Mode == OFF) { light0Mode = ON; }
		else { light0Mode = OFF; }
		break;
	case 'L':
		if (light1Mode == OFF) { light1Mode = ON; }
		else { light1Mode = OFF; }
		break;
		//Light Rotation
	case '<':
		lightX -= 5;
		break;
	case '>':
		lightX += 5;
		break;

		//Light Elevation
	case '[':
		lightY -= 0.5;
		break;
	case ']':
		lightY += 0.5;
		break;

		//Light Distance
	case '{':
		lightDistance -= 1;
		break;
	case '}':
		lightDistance += 1;
		break;
		//Light Ambient Level
	case 'h':
		lightAmbient -= 5;
		break;
	case 'H':
		lightAmbient += 5;
		break;

		//Light Diffuse Level
	case 'j':
		lightDiffuse -= 5;
		break;
	case 'J':
		lightDiffuse += 5;
		break;

		//Light Specular Level
	case 'k':
		lightSpecular -= 5;
		break;
	case 'K':
		lightSpecular += 5;
		break;

		//Light Shininess Level
	case 'u':
		lightShininess -= 1;
		break;
	case 'U':
		lightShininess += 1;
		break;

		//Material Ambient Light
	case 'n':
		materialAmbient -= 5;
		break;
	case 'N':
		materialAmbient += 5;
		break;
		//Material Diffuse Light
	case 'c':
		materialDiffuse -= 5;
		break;
	case 'C':
		materialDiffuse += 5;
		break;
		//Material Specular Light	
	case 'v':
		materialSpecular -= 5;
		break;
	case 'V':
		materialSpecular += 5;
		break;

		//Global Ambient Light
	case 'o':
		globalAmbientLight += 5;
		break;
	case 'O':
		globalAmbientLight -= 5;
		break;
		//Shade Mode
	case 'q':
		if (shadeType == FLAT) { shadeType = SMOOTH; }
		else { shadeType = FLAT; }
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

		//Reset
	case 'r':
		xAngle = 0;
		yAngle = 0;
		zAngle = 0;
		xTranslate = 0;
		yTranslate = 0;
		zTranslate = 0;
		dim = 4;
		fieldOfView = 55;
		scale();
		aspectRatio = 1;
		far = 4;
		near = 4;
		break;

	default:
		break;
	}

	//Translate Shininess power
	shinyVector[0] = lightShininess < 0 ? 0 : pow(2.0, lightShininess);
	project();
	glutPostRedisplay();
}

/*Arrow Key Functionality to Move the Display for the Object*/
/*Used for Precise Rotation!*/
void windowSpecial(int key, int x, int y) {
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
		if (state == GLUT_DOWN) {
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

	initializeTextures();

	glutMainLoop();
	return 0;

}