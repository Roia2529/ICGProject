#pragma once
#ifndef GLFUNC_H
#define GLFUNC_H

#include "glHeader.h"
#include <stdio.h>
#include <time.h>

bool initGL();
/*
Pre Condition:
-A valid OpenGL context
Post Condition:
-Initializes matrices and clear color
-Reports to console if there was an OpenGL error
-Returns false if there was an error in initialization
Side Effects:
-Projection matrix is set to identity matrix
-Modelview matrix is set to identity matrix
-Matrix mode is set to modelview
-Clear color is set to black
*/

void GLupdate();
/*
Pre Condition:
-None
Post Condition:
-Does per frame logic
Side Effects:
-None
*/

void GLrender();
/*
Pre Condition:
-A valid OpenGL context
-Active modelview matrix
Post Condition:
-Renders the scene
Side Effects:
-Clears the color buffer
-Swaps the front/back buffer
*/

void GLkeyboardInput(unsigned char key, int x, int y);

void GLidle();

#endif