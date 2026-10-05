
// Application
#include "C3DScene.h"
#include "CMan.h"

//-------------------------------------------------------------------------------------------------

CComponent* CMan::instantiator(C3DScene* pScene)
{
    return new CMan(pScene);
}

//-------------------------------------------------------------------------------------------------

CMan::CMan(C3DScene* pScene)
    : CTrajectorable(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CMan::~CMan()
{
}

//-------------------------------------------------------------------------------------------------

void CMan::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CMesh::loadParameters(sBaseFile, xComponent);
}
