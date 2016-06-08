
// Fondations
#include "CLogManager.h"

// Application
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CArmature.h"
#include "CBone.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CBone::instanciator(C3DScene* pScene)
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

void CBone::loadParameters(CXMLNode xComponent)
{
}
