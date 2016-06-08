
// Fondations
#include "CLogManager.h"

// Application
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CArmature.h"

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

void CArmature::loadParameters(CXMLNode xComponent)
{
    CPhysicalComponent::loadParameters(xComponent);

    CXMLNode xParameters = xComponent.getNodeByTagName(ParamName_Parameters);
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
