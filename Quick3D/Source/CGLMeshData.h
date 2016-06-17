
#pragma once

#include "quick3d_global.h"

// Qt
#include <QSharedPointer>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "CVertex.h"
#include "CFace.h"
#include "CMaterial.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGLMeshData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGLMeshData(C3DScene* pScene);

    //!
    ~CGLMeshData();

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void paint(CRenderContext* pContext, const QMatrix4x4& mModelAbsolute, QGLShaderProgram* pProgram, int iGLType);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    // Properties
    //-------------------------------------------------------------------------------------------------

    C3DScene*       m_pScene;
    GLuint          m_iNumRenderPoints;         // Nombre de sommets transf�r�s � OpenGL
    GLuint          m_iNumRenderIndices;        // Nombre d'indices de sommets des polygones transf�r�s � OpenGL
    CVertex*        m_vRenderPoints;            // Sommets transf�r�s � OpenGL
    GLuint*         m_vRenderIndices;           // Indices de sommets des polygones transf�r�s � OpenGL
    GLuint          m_iVBO [2];                 // Buffers de donn�es allou�s par OpenGL
    int             m_iGLType;
    bool            m_bNeedTransferBuffers;     // Si vrai, il est temps de donner � OpenGL les buffers de g�om�trie

    static GLuint   m_iCurrentVBO;              // Num�ro du dernier VBO transmis � OpenGL
};
