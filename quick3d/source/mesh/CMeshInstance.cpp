
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CMeshInstance::CMeshInstance(QSP<CMesh> pMesh)
    : CComponent(pMesh->scene())
{
    CComponent::incComponentCounter(ClassName_CMeshInstance);

    m_vMeshes.append(pMesh);
}

//-------------------------------------------------------------------------------------------------

CMeshInstance::CMeshInstance(const QVector<QSP<CMesh> >& vMeshes)
    : CComponent(vMeshes[0]->scene())
{
    CComponent::incComponentCounter(ClassName_CMeshInstance);

    foreach (QSP<CMesh> pMesh, vMeshes)
    {
        m_vMeshes.append(pMesh);
    }
}

//-------------------------------------------------------------------------------------------------

CMeshInstance::~CMeshInstance()
{
    CComponent::decComponentCounter(ClassName_CMeshInstance);
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

CBoundingBox CMeshInstance::bounds()
{
    if (m_vMeshes.count() > 0)
    {
        return m_vMeshes[0]->bounds();
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMeshInstance::worldBounds()
{
    if (m_vMeshes.count() > 0)
    {
        CBoundingBox bBounds(m_vMeshes[0]->bounds());
        CVector3 vWorldPosition = worldPosition();
        return CBoundingBox(vWorldPosition + bBounds.minimum(), vWorldPosition + bBounds.maximum());
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

void CMeshInstance::paint(CRenderContext* pContext)
{
    if (m_vMeshes.count() > 0)
    {
        CVector3 vPosition = pContext->internalCameraMatrix() * worldBounds().center();

        foreach (QSP<CMesh> pMesh, m_vMeshes)
        {
            if (vPosition.magnitude() <= pMesh->geometry()->maxDistance())
            {
                pMesh->setWorldTransform(m_mWorldTransform);
                pMesh->paint(pContext);

                // Paint only this LOD
                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshInstance::dump(QTextStream& stream, int iIdent)
{
    dumpIndented(stream, iIdent, QString("[CMeshInstance]"));
    dumpIndented(stream, iIdent, QString("Meshes :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    foreach (QSP<CMesh> pMesh, m_vMeshes)
    {
        pMesh->dump(stream, iIdent);
    }
    iIdent--; dumpCloseBlock(stream, iIdent);

    CComponent::dump(stream, iIdent);
}
