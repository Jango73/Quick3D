
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CVertex.h"
#include "CFace.h"
#include "CMaterial.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

class QUICK3D_EXPORT CGLMeshData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGLMeshData(C3DScene* pScene);

    //!
    ~CGLMeshData();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void paint(CRenderContext* pContext, const QMatrix4x4& mModelAbsolute, QGLShaderProgram* pProgram, int iGLType);

    //-------------------------------------------------------------------------------------------------
    // Properties
    // Properties
    //-------------------------------------------------------------------------------------------------

    C3DScene*       m_pScene;
    GLuint          m_iNumRenderPoints;         // Nombre de sommets transférés à OpenGL
    GLuint          m_iNumRenderIndices;        // Nombre d'indices de sommets des polygones transférés à OpenGL
    CVertex*        m_vRenderPoints;            // Sommets transférés à OpenGL
    GLuint*         m_vRenderIndices;           // Indices de sommets des polygones transférés à OpenGL
    GLuint          m_iVBO [2];                 // Buffers de données alloués par OpenGL
    int             m_iGLType;
    bool            m_bNeedTransferBuffers;     // Si vrai, il est temps de donner à OpenGL les buffers de géométrie

    static GLuint   m_iCurrentVBO;              // Numéro du dernier VBO transmis à OpenGL
};
