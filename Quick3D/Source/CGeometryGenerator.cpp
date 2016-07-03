
// Application
#include "CLogManager.h"
#include "C3DScene.h"
#include "CGeometryGenerator.h"

//-------------------------------------------------------------------------------------------------

CComponent* CGeometryGenerator::instanciator(C3DScene* pScene)
{
    return new CGeometryGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CGeometryGenerator::CGeometryGenerator(C3DScene* pScene)
: CComponent(pScene)
{
    LOG_DEBUG("CGeometryGenerator::CGeometryGenerator()");
}

//-------------------------------------------------------------------------------------------------

CGeometryGenerator::~CGeometryGenerator()
{
    LOG_DEBUG("CGeometryGenerator::~CGeometryGenerator()");
}
