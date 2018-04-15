
// Application
#include "CParticleSystem.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CParticleSystem::CParticleSystem(C3DScene* pScene)
    : CMesh(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CParticleSystem::~CParticleSystem()
{
}

//-------------------------------------------------------------------------------------------------

void CParticleSystem::update(double dDeltaTime)
{
    CMesh::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CParticleSystem::paint(CRenderContext* pContext)
{
    /*
    m_vVertices.clear();
    m_vFaces.clear();

    foreach (CParticle p, m_vParticles)
    {
        int iVertexOffset = m_vVertices.count();

        CVector3 vPosition = p.position();
        CVector3 vVelocity = p.velocity();
        CVector2 vSize(0.03, vVelocity.Y * 0.2);

        CVector3 v1 = vPosition + CVector3(-vSize.X, -vSize.Y, 0.0);
        CVector3 v2 = vPosition + CVector3( vSize.X, -vSize.Y, 0.0);
        CVector3 v3 = vPosition + CVector3( vSize.X,  vSize.Y, 0.0);
        CVector3 v4 = vPosition + CVector3(-vSize.X,  vSize.Y, 0.0);

        // Back
        m_vVertices.append(CVertex(v1));
        m_vVertices.append(CVertex(v2));
        m_vVertices.append(CVertex(v3));
        m_vVertices.append(CVertex(v4));

        // Front
        m_vVertices.append(CVertex(v4));
        m_vVertices.append(CVertex(v3));
        m_vVertices.append(CVertex(v2));
        m_vVertices.append(CVertex(v1));

        m_vFaces.append(CFace(this, iVertexOffset + 0, iVertexOffset + 1, iVertexOffset + 2, iVertexOffset + 3));
        m_vFaces.append(CFace(this, iVertexOffset + 4, iVertexOffset + 5, iVertexOffset + 6, iVertexOffset + 7));
    }

    setGeometryDirty(true);

    CMesh::paint(pContext);
    */
}
