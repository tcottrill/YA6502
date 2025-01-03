/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#pragma once

#ifndef GL_BASICS_H
#define GL_BASICS_H
#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"
#include "wglext.h"
#include <string>

//Texture helper structure, this is the basis for a little mini texture manager I use in another project.

typedef struct
{
	GLuint texid;
	int width;
	int height;
	int bpp;
	std::string name;
} TEX;

void CheckGLVersionSupport();
void ViewOrtho(int width, int height);
void SetVSync(bool sync);
TEX *LoadPNG(std::string filename);
void UseTexture(GLuint *texture, GLboolean linear, GLboolean mipmapping);
void SetBlendMode(int mode);
void FreeTexture( GLuint fonttex );
void FreeTex(TEX *fonttex);
void CreateGLContext();
void DeleteGLContext();
void GlSwap();
void SnapShot();
GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
void GLRect(int xmin, int xmax, int ymin, int ymax);
void GLPoint(float x,float y);
void GLLine(float sx, float sy, float ex, float ey);

#endif