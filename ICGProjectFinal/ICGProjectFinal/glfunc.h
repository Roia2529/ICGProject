#pragma once
#ifndef GLFUNC_H
#define GLFUNC_H

#include "glHeader.h"
#include "constant.h"
#include <stdio.h>
#include <time.h>

bool LoadObj(const char *filename, bool loadMtl);

bool initShaders();
void prepareMatrix(GLfloat scaleinit);
void glBufferBind();
void updateView();
cy::Matrix4f OrthographicProj(float aspect, float x, float y, float n, float f, bool fixaspect);
cy::Matrix4f LookAt(cy::Point3f campos, cy::Point3f focus, cy::Point3f up);

bool initGLRenderTexture(int width, int height);
bool initShadowMap(int width, int height);

void GLrender();

//mouse event
void GLGetMouseButton(GLint button, GLint state, GLint x, GLint y);
void GLmouseMotion(GLint x, GLint y);

//keyboard event
void GLkeyboardInput(unsigned char key, int x, int y);
void GLGetSpecialKey(int key, int x, int y);

void GLidle();

#endif