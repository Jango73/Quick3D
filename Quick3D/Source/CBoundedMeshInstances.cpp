
#include "C3DScene.h"
#include "CBoundedMeshInstances.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBoundedMeshInstances::CBoundedMeshInstances(C3DScene* pScene)
    : CComponent(pScene)
{
    CComponent::incComponentCounter(ClassName_CBoundedMeshInstances);
}

//-------------------------------------------------------------------------------------------------

CBoundedMeshInstances::~CBoundedMeshInstances()
{
    CComponent::decComponentCounter(ClassName_CBoundedMeshInstances);

    foreach (CMeshInstance* pMesh, m_vMeshes)
    {
        if (pMesh != nullptr)
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

CBoundingBox CBoundedMeshInstances::bounds()
{
    return m_bBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundedMeshInstances::worldBounds()
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
    CVector3 vPosition = pContext->internalCameraMatrix() * worldBounds().center();
    double dRadius = worldBounds().radius();

    if (pContext->scene()->frustumCheck() == false || pContext->camera()->contains(vPosition, dRadius))
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
    if (pMeshInstance != nullptr && m_vMeshes.contains(pMeshInstance) == false)
    {
        m_vMeshes.append(pMeshInstance);
    }
}

//-------------------------------------------------------------------------------------------------

void CBoundedMeshInstances::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CBoundedMeshInstances]"));
    dumpIdent(stream, iIdent, QString("Meshes :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    foreach (CMeshInstance* pMesh, m_vMeshes)
    {
        pMesh->dump(stream, iIdent);
    }
    iIdent--; dumpCloseBlock(stream, iIdent);

    CComponent::dump(stream, iIdent);
}
