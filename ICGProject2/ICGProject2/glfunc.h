#pragma once
#ifndef GLFUNC_H
#define GLFUNC_H

#include "glHeader.h"
#include <stdio.h>
#include <time.h>

bool LoadObj(const char *filename, bool loadMtl);

void glBufferBind();

bool initGL();

void GLupdate();


void GLrender();

//mouse event

//keyboard event
void GLkeyboardInput(unsigned char key, int x, int y);
void GLGetSpecialKey(int key, int x, int y)

void GLidle();

#endif