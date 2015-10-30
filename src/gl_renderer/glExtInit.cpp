#include <stdio.h>
#include <string.h>

//#include "GL/glcorearb.h"
#include "glExtInit.h"

// GL_VERSION_1.3
bool IsExtEnable(const char * pName)
{
    // Получить строку со списком доступных расширений
    const char * str = (const char *)glGetString(GL_EXTENSIONS);

    size_t szName = strlen( pName );
    size_t szStr = strlen( str );
    for( size_t i = 0; i < szStr - szName; i ++ )
    {
        const char * ch = &str[i];
        if( strncmp( ch, pName, szName ) == 0 )
            return true;
    }

    return false;
}

bool Init_GLVERSION15(void)
{
    bool rez = true;
#ifdef _WIN32
    glClientActiveTexture = ( PFNGLCLIENTACTIVETEXTUREPROC ) wglGetProcAddress( "glClientActiveTexture" );
    rez = ( glClientActiveTexture==nullptr) ? false : true;
#endif
    return rez;
}
// end of GL_VERSION_1.3

// GL_VERSION_1.5
/*
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
*/

bool Init_VertexBufferObject(void)
{
    //if( !IsExtEnable( "ARB_vertex_buffer_object" ) )
    //	return false;
    bool rez = true;

#ifdef _WIN32
    glGenBuffers = ( PFNGLGENBUFFERSPROC ) wglGetProcAddress( "glGenBuffers" );
    glBindBuffer = ( PFNGLBINDBUFFERPROC ) wglGetProcAddress( "glBindBuffer" );
    glBufferData = ( PFNGLBUFFERDATAPROC ) wglGetProcAddress( "glBufferData" );
    rez = glGenBuffers && glBindBuffer && glBufferData && glDeleteBuffers;
#endif
#ifdef __linux
    //glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glXGetProcAddress?("glDeleteBuffers");
    //void *ptr = glXGetProcAddress?("glDeleteBuffers");
#endif
    return rez;
}
// end of GL_VERSION_1.5

// GL_VERSION_2.0
/*
PFNGLVERTEXATTRIBPOINTERPROC	 glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLBINDATTRIBLOCATIONPROC		 glBindAttribLocation;
PFNGLGETATTRIBLOCATIONPROC		 glGetAttribLocation;
PFNGLGETACTIVEATTRIBPROC         glGetActiveAttrib;
PFNGLGETUNIFORMLOCATIONPROC		 glGetUniformLocation;
PFNGLUNIFORM1FPROC				 glUniform1f;
*/

bool Init_VertexArray( void )
{
    //if( !IsExtEnable( "EXT_vertex_array" ) )
    //	return false;
    bool rez = true;

#ifdef _WIN32
    glVertexAttribPointer = ( PFNGLVERTEXATTRIBPOINTERPROC ) wglGetProcAddress( "glVertexAttribPointer" );
    glEnableVertexAttribArray = ( PFNGLENABLEVERTEXATTRIBARRAYPROC ) wglGetProcAddress( "glEnableVertexAttribArray" );
    glBindAttribLocation = ( PFNGLBINDATTRIBLOCATIONPROC ) wglGetProcAddress( "glBindAttribLocation" );
    glGetAttribLocation = ( PFNGLGETATTRIBLOCATIONPROC ) wglGetProcAddress( "glGetAttribLocation" );
    glGetActiveAttrib = ( PFNGLGETACTIVEATTRIBPROC ) wglGetProcAddress( "glGetActiveAttrib" );
    glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC ) wglGetProcAddress( "glGetUniformLocation" );
    glUniform1f = ( PFNGLUNIFORM1FPROC ) wglGetProcAddress( "glUniform1f" );
    rez = glVertexAttribPointer && glEnableVertexAttribArray && glBindAttribLocation && glGetAttribLocation && glGetActiveAttrib && glGetUniformLocation;
#endif

    return rez;
}

/*
PFNGLCREATESHADERPROC	glCreateShader;
PFNGLSHADERSOURCEPROC	glShaderSource;
PFNGLCOMPILESHADERPROC	glCompileShader;
PFNGLCREATEPROGRAMPROC	glCreateProgram;
PFNGLATTACHSHADERPROC	glAttachShader;
PFNGLLINKPROGRAMPROC	glLinkProgram;
PFNGLUSEPROGRAMPROC		glUseProgram;
PFNGLGETSHADERIVPROC	glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC		glGetProgramiv;
*/

bool Init_ShaderObjects( void )
{
    //if( !IsExtEnable( "ARB_shader_objects" ) )
    //	return false;

    bool rez = true;

#ifdef _WIN32
    glCreateShader = ( PFNGLCREATESHADERPROC ) wglGetProcAddress( "glCreateShader" );
    glShaderSource = ( PFNGLSHADERSOURCEPROC ) wglGetProcAddress( "glShaderSource" );
    glCompileShader = ( PFNGLCOMPILESHADERPROC ) wglGetProcAddress( "glCompileShader" );
    glCreateProgram = ( PFNGLCREATEPROGRAMPROC ) wglGetProcAddress( "glCreateProgram" );
    glAttachShader = ( PFNGLATTACHSHADERPROC ) wglGetProcAddress( "glAttachShader" );
    glLinkProgram = ( PFNGLLINKPROGRAMPROC ) wglGetProcAddress( "glLinkProgram" );
    glUseProgram = ( PFNGLUSEPROGRAMPROC ) wglGetProcAddress( "glUseProgram" );
    glGetShaderiv = ( PFNGLGETSHADERIVPROC ) wglGetProcAddress( "glGetShaderiv" );
    glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC ) wglGetProcAddress( "glGetShaderInfoLog" );
    glGetProgramiv = ( PFNGLGETPROGRAMIVPROC ) wglGetProcAddress( "glGetProgramiv" );
    rez = glCreateShader && glShaderSource && glCompileShader && glCreateProgram && glAttachShader && glLinkProgram && glUseProgram && glGetShaderiv && glGetShaderInfoLog && glGetProgramiv;
#endif

    return rez;
}
// end of GL_VERSION_2.0
