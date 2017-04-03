#pragma once
#ifndef OPENGLDEFAULTS
#define OPENGLDEFAULTS
#define WIN32
#define _CRT_SECURE_NO_DEPRECATE

/*Standards*/
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>

/*OpenGL and Common*/
#include<vector>
#ifdef USEGLEW
#include <GL/glut.h>
#endif
#define GL_GLECT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#endif



