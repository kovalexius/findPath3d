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

#if (_MSC_VER == 1600) || (_MSC_VER == 1800 )
#define OGL_EXT
#endif

#ifdef OGL_EXT
// GL_VERSION_1.3
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
// end of GL_VERSION_1.3

// GL_VERSION_1.5
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
// end of GL_VERSION_1.5

// GL_VERSION_2.0
extern PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLBINDATTRIBLOCATIONPROC		glBindAttribLocation;
extern PFNGLGETATTRIBLOCATIONPROC		glGetAttribLocation;
extern PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib;
extern PFNGLGETUNIFORMLOCATIONPROC		glGetUniformLocation;
extern PFNGLUNIFORM1FPROC				glUniform1f;


extern PFNGLCREATESHADERPROC   glCreateShader;
extern PFNGLSHADERSOURCEPROC	  glShaderSource;
extern PFNGLCOMPILESHADERPROC	glCompileShader;
extern PFNGLCREATEPROGRAMPROC	glCreateProgram;
extern PFNGLATTACHSHADERPROC	  glAttachShader;
extern PFNGLLINKPROGRAMPROC	  glLinkProgram;
extern PFNGLUSEPROGRAMPROC		  glUseProgram;
extern PFNGLGETSHADERIVPROC	  glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMIVPROC	  glGetProgramiv;
// end of GL_VERSION_2.0

bool Init_GLVERSION15(void);
bool Init_ShaderObjects(void);
bool Init_VertexBufferObject(void);
bool Init_VertexArray(void);

#endif

#endif
