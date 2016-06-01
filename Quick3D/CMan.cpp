
// Application
#include "C3DScene.h"
#include "CMan.h"

//-------------------------------------------------------------------------------------------------

CComponent* CMan::instanciator(C3DScene* pScene)
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

void CMan::loadParameters(CXMLNode xComponent)
{
	CMesh::loadParameters(xComponent);
}
