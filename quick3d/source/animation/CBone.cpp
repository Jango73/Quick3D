
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CArmature.h"
#include "CBone.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CBone::instantiator(C3DScene* pScene)
{
    return new CBone(pScene);
}

//-------------------------------------------------------------------------------------------------

CBone::CBone(C3DScene *pScene)
    : CComponent(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CBone::~CBone()
{
}

//-------------------------------------------------------------------------------------------------

void CBone::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xHeadNode = xComponent.getNodeByTagName("Head");
    CXMLNode xTailNode = xComponent.getNodeByTagName("Tail");
    CXMLNode xXAxisNode = xComponent.getNodeByTagName("XAxis");
    CXMLNode xYAxisNode = xComponent.getNodeByTagName("YAxis");
    CXMLNode xZAxisNode = xComponent.getNodeByTagName("ZAxis");
}
