#pragma once
#ifndef GLFUNC_H
#define GLFUNC_H

#include "glHeader.h"
#include "constant.h"
#include <stdio.h>
#include <time.h>

bool LoadObj(const char *filename, bool loadMtl);
bool glinitGLSLProgram(const char *vShaderFile, const char *fShaderFile);
bool glinitRT_GLSLProgram(const char *vShaderFile, const char *fShaderFile);

void prepareMatrix(GLfloat scaleinit);
void glBufferBind();

bool initGL();

bool initGLRenderTexture(int width, int height);

void GLrender();

//mouse event
void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y);
void GLmouseMotion(GLint x, GLint y);

//keyboard event
void GLkeyboardInput(unsigned char key, int x, int y);
void GLGetSpecialKey(int key, int x, int y);

void GLidle();

#endif