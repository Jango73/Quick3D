
#pragma once

// Qt
#include <QtOpenGL>

// OpenGL
#ifdef WIN32
    #include "glext.h"
#else
    #include <GL/glext.h>
#endif

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#define GL_glGenBuffers                 m_pScene->glExtension()->glGenBuffers
#define GL_glDeleteBuffers              m_pScene->glExtension()->glDeleteBuffers
#define GL_glBindBuffer                 m_pScene->glExtension()->glBindBuffer
#define GL_glBufferData                 m_pScene->glExtension()->glBufferData
#define GL_glGetAttribLocation          m_pScene->glExtension()->glGetAttribLocation
#define GL_glEnableVertexAttribArray    m_pScene->glExtension()->glEnableVertexAttribArray
#define GL_glDisableVertexAttribArray   m_pScene->glExtension()->glDisableVertexAttribArray
#define GL_glVertexAttribPointer        m_pScene->glExtension()->glVertexAttribPointer
#define GL_glActiveTexture              m_pScene->glExtension()->glActiveTexture
#define GL_glGenerateMipmap             m_pScene->glExtension()->glGenerateMipmap

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
