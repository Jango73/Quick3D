
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

    CGLMeshData(C3DScene* pScene);

    ~CGLMeshData();

    void paint(CRenderContext* pContext, const QMatrix4x4& mModelAbsolute, QGLShaderProgram* pProgram, bool bAllQuads, bool bPointCloud);

    C3DScene*       m_pScene;
    GLuint          m_iNumRenderPoints;			// Nombre de sommets transf�r�s � OpenGL
    GLuint          m_iNumRenderIndices;		// Nombre d'indices de sommets des polygones transf�r�s � OpenGL
    CVertex*        m_vRenderPoints;			// Sommets transf�r�s � OpenGL
    GLuint*         m_vRenderIndices;			// Indices de sommets des polygones transf�r�s � OpenGL
    GLuint          m_iVBO [2];					// Buffers de donn�es allou�s par OpenGL
    bool            m_bNeedTransferBuffers;		// Si vrai, il est temps de donner � OpenGL les buffers de g�om�trie

    static GLuint   m_iCurrentVBO;				// Num�ro du dernier VBO transmis � OpenGL
};
