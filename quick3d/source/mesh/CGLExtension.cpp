
// Application
#include "CGLExtension.h"

#ifndef WIN32
#include <GL/glx.h>
#endif

//-------------------------------------------------------------------------------------------------

CGLExtension::CGLExtension()
{
#ifdef WIN32
	glGenBuffers				= (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	glDeleteBuffers				= (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
	glBindBuffer				= (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
	glBufferData				= (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
	glGetAttribLocation			= (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress("glGetAttribLocation");
	glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
	glVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
	glActiveTexture				= (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
	glGenerateMipmap			= (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
#else
    glGenBuffers				= (PFNGLGENBUFFERSPROC) glXGetProcAddress((const GLubyte *)("glGenBuffers"));
    glDeleteBuffers				= (PFNGLDELETEBUFFERSPROC) glXGetProcAddress((const GLubyte *)("glDeleteBuffers"));
    glBindBuffer				= (PFNGLBINDBUFFERPROC) glXGetProcAddress((const GLubyte *)("glBindBuffer"));
    glBufferData				= (PFNGLBUFFERDATAPROC) glXGetProcAddress((const GLubyte *)("glBufferData"));
    glGetAttribLocation			= (PFNGLGETATTRIBLOCATIONPROC) glXGetProcAddress((const GLubyte *)("glGetAttribLocation"));
    glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC) glXGetProcAddress((const GLubyte *)("glEnableVertexAttribArray"));
    glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC) glXGetProcAddress((const GLubyte *)("glDisableVertexAttribArray"));
    glVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC) glXGetProcAddress((const GLubyte *)("glVertexAttribPointer"));
    glActiveTexture				= (PFNGLACTIVETEXTUREPROC) glXGetProcAddress((const GLubyte *)("glActiveTexture"));
    glGenerateMipmap			= (PFNGLGENERATEMIPMAPPROC) glXGetProcAddress((const GLubyte *)("glGenerateMipmap"));
#endif
}

//-------------------------------------------------------------------------------------------------

CGLExtension::~CGLExtension()
{
}
