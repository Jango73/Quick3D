
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CMeshInstance::CMeshInstance(QSP<CMesh> pMesh)
    : CComponent(pMesh->getScene())
{
    m_vMeshes.append(pMesh);
}

//-------------------------------------------------------------------------------------------------

CMeshInstance::CMeshInstance(const QVector<QSP<CMesh> >& vMeshes)
    : CComponent(vMeshes[0]->getScene())
{
    foreach (QSP<CMesh> pMesh, vMeshes)
    {
        m_vMeshes.append(pMesh);
    }
}

//-------------------------------------------------------------------------------------------------

CMeshInstance::~CMeshInstance()
{
}

//-------------------------------------------------------------------------------------------------

void CMeshInstance::addMesh(QSP<CMesh> pMesh)
{
    m_vMeshes.append(pMesh);
}

//-------------------------------------------------------------------------------------------------

CMeshInstance* CMeshInstance::clone()
{
    return new CMeshInstance(m_vMeshes);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMeshInstance::getBounds()
{
    if (m_vMeshes.count() > 0)
    {
        return m_vMeshes[0]->getBounds();
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMeshInstance::getWorldBounds()
{
    if (m_vMeshes.count() > 0)
    {
        CBoundingBox bBounds(m_vMeshes[0]->getBounds());
        CVector3 vWorldPosition = getWorldPosition();
        return CBoundingBox(vWorldPosition + bBounds.minimum(), vWorldPosition + bBounds.maximum());
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

void CMeshInstance::paint(CRenderContext* pContext)
{
    if (m_vMeshes.count() > 0)
    {
        CVector3 vPosition = pContext->internalCameraMatrix() * getWorldBounds().center();

        foreach (QSP<CMesh> pMesh, m_vMeshes)
        {
            if (vPosition.getMagnitude() <= pMesh->geometry()->getMaxDistance())
            {
                pMesh->setWorldTransform(m_mWorldTransform);
                pMesh->paint(pContext);

                // Paint only this LOD
                return;
            }
        }
    }
}
