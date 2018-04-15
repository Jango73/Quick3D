
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// qt-plus
#include "CMemoryMonitor.h"

// Application
#include "quick3d_global.h"
#include "CVertex.h"
#include "CFace.h"
#include "CMaterial.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGLMeshData
{
    DECLARE_MEMORY_MONITORED

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGLMeshData(C3DScene* pScene);

    //!
    ~CGLMeshData();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void paint(CRenderContext* pContext, const QMatrix4x4& mModelAbsolute, QGLShaderProgram* pProgram, int iGLType);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    C3DScene*       m_pScene;
    GLuint          m_iNumRenderPoints;         // Number of vertices transfered to OpenGL
    GLuint          m_iNumRenderIndices;        // Number of polygon indices transfered to OpenGL
    CVertex*        m_vRenderPoints;            // Vertices transfered to OpenGL
    GLuint*         m_vRenderIndices;           // Polygon vertex indices transfered to OpenGL
    GLuint          m_iVBO [2];                 // Data bufers allocated by OpenGL
    int             m_iGLType;
    bool            m_bNeedTransferBuffers;     // If true, it is time to give OpenGL the geometry buffers

    static GLuint   m_iCurrentVBO;              // Last VBO transmitted to OpenGL
};
