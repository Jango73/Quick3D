
#include "C3DScene.h"
#include "CBoundedMeshInstances.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBoundedMeshInstances::CBoundedMeshInstances(C3DScene* pScene)
    : CComponent(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CBoundedMeshInstances::~CBoundedMeshInstances()
{
    foreach (CMeshInstance* pMesh, m_vMeshes)
    {
        if (pMesh != NULL)
        {
            delete pMesh;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CBoundedMeshInstances::setBounds(CBoundingBox bBounds)
{
    m_bBounds = bBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundedMeshInstances::getBounds() const
{
    return m_bBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundedMeshInstances::getWorldBounds() const
{
    return m_bBounds;
}

//-------------------------------------------------------------------------------------------------

void CBoundedMeshInstances::update(double dDeltaTime)
{
    foreach (CMeshInstance* pMeshInstance, m_vMeshes)
    {
        pMeshInstance->update(dDeltaTime);
    }
}

//-------------------------------------------------------------------------------------------------

void CBoundedMeshInstances::paint(CRenderContext* pContext)
{
    CVector3 vPosition = pContext->internalCameraMatrix() * getWorldBounds().center();
    double dRadius = getWorldBounds().radius();

    if (pContext->scene()->getFrustumCheck() == false || pContext->camera()->contains(vPosition, dRadius))
    {
        foreach (CMeshInstance* pMeshInstance, m_vMeshes)
        {
            pMeshInstance->paint(pContext);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CBoundedMeshInstances::add(CMeshInstance* pMeshInstance)
{
    if (pMeshInstance != NULL && m_vMeshes.contains(pMeshInstance) == false)
    {
        m_vMeshes.append(pMeshInstance);
    }
}
