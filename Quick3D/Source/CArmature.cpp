
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CArmature.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CArmature::instantiator(C3DScene* pScene)
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

void CArmature::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CArmature::update(double dDeltaTime)
{
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        QSP<CMesh> pMesh = QSP_CAST(CMesh, pChild);

        if (pMesh != nullptr)
        {
            updateSingleMesh(pMesh, dDeltaTime);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CArmature::updateSingleMesh(QSP<CMesh> pMesh, double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

void CArmature::paint(CRenderContext* pContext)
{
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CArmature::intersect(Math::CRay3 ray)
{
    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CArmature::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CArmature]"));

    CPhysicalComponent::dump(stream, iIdent);
}
