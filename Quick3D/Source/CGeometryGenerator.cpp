
// Application
#include "CLogManager.h"
#include "C3DScene.h"
#include "CGeometryGenerator.h"

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

//-------------------------------------------------------------------------------------------------

void CGeometryGenerator::loadParameters(const QString& sBaseFile, CXMLNode xComponent, CXMLNode xFunctions)
{
}
