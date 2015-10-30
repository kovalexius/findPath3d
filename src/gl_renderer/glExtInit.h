#ifndef GLEXTINIT_H
#define GLEXTINIT_H

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __linux
#include <GL/glx.h>
#include <GL/glxext.h>
#endif
#include <GL/gl.h>
#include "GL/glu.h"
#include "GL/glext.h"

#ifdef _WIN32
// GL_VERSION_1.3
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
// end of GL_VERSION_1.3

// GL_VERSION_1.5
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
// end of GL_VERSION_1.5

// GL_VERSION_2.0
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLBINDATTRIBLOCATIONPROC		glBindAttribLocation;
PFNGLGETATTRIBLOCATIONPROC		glGetAttribLocation;
PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib;
PFNGLGETUNIFORMLOCATIONPROC		glGetUniformLocation;
PFNGLUNIFORM1FPROC				glUniform1f;


PFNGLCREATESHADERPROC	glCreateShader;
PFNGLSHADERSOURCEPROC	glShaderSource;
PFNGLCOMPILESHADERPROC	glCompileShader;
PFNGLCREATEPROGRAMPROC	glCreateProgram;
PFNGLATTACHSHADERPROC	glAttachShader;
PFNGLLINKPROGRAMPROC		glLinkProgram;
PFNGLUSEPROGRAMPROC		glUseProgram;
PFNGLGETSHADERIVPROC		glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC	 glGetProgramiv;
// end of GL_VERSION_2.0
#endif

// GL_VERSION_1.5
//PFNGLDELETEBUFFERSPROC glDeleteBuffers;
// end of GL_VERSION_1.5

bool Init_GLVERSION15(void);
bool Init_ShaderObjects( void );
bool Init_VertexBufferObject(void);
bool Init_VertexArray( void );

#endif
