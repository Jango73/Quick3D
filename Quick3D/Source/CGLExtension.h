
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QtOpenGL>

// OpenGL
#ifdef WIN32
    #include "glext.h"
#else
    #include <GL/glext.h>
#endif

//-------------------------------------------------------------------------------------------------

#define GL_glGenBuffers                 m_pScene->getGLExtension()->glGenBuffers
#define GL_glDeleteBuffers              m_pScene->getGLExtension()->glDeleteBuffers
#define GL_glBindBuffer                 m_pScene->getGLExtension()->glBindBuffer
#define GL_glBufferData                 m_pScene->getGLExtension()->glBufferData
#define GL_glGetAttribLocation          m_pScene->getGLExtension()->glGetAttribLocation
#define GL_glEnableVertexAttribArray    m_pScene->getGLExtension()->glEnableVertexAttribArray
#define GL_glDisableVertexAttribArray   m_pScene->getGLExtension()->glDisableVertexAttribArray
#define GL_glVertexAttribPointer        m_pScene->getGLExtension()->glVertexAttribPointer
#define GL_glActiveTexture              m_pScene->getGLExtension()->glActiveTexture
#define GL_glGenerateMipmap             m_pScene->getGLExtension()->glGenerateMipmap

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGLExtension
{
public:

	//!
	CGLExtension();

	//!
	~CGLExtension();

	PFNGLGENBUFFERSPROC					glGenBuffers;
	PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
	PFNGLBINDBUFFERPROC					glBindBuffer;
	PFNGLBUFFERDATAPROC					glBufferData;
	PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation;
	PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
	PFNGLACTIVETEXTUREPROC				glActiveTexture;
    PFNGLGENERATEMIPMAPPROC				glGenerateMipmap;
};
