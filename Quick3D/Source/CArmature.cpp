
// Fondations
#include "CLogManager.h"

// Application
#include "C3DScene.h"
#include "CArmature.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CArmature::instanciator(C3DScene* pScene)
{
    return new CArmature(pScene);
}

//-------------------------------------------------------------------------------------------------

CArmature::CArmature(C3DScene* pScene)
    : CPhysicalComponent(pScene)
{
    setName("Armature");
}

//-------------------------------------------------------------------------------------------------

CArmature::~CArmature()
{
}

//-------------------------------------------------------------------------------------------------

void CArmature::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CArmature::update(double dDeltaTime)
{
    foreach (CComponent* pChild, m_vChildren)
    {
        CMesh* pMesh = dynamic_cast<CMesh*>(pChild);

        if (pMesh != NULL)
        {
            updateSingleMesh(pMesh, dDeltaTime);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CArmature::updateSingleMesh(CMesh* pMesh, double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

void CArmature::paint(CRenderContext* pContext)
{
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CArmature::intersect(Math::CRay3 ray) const
{
    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CArmature::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CArmature]"));

    CPhysicalComponent::dump(stream, iIdent);
}
