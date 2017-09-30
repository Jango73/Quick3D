
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CGeometryGenerator.h"

//-------------------------------------------------------------------------------------------------

CGeometryGenerator::CGeometryGenerator(C3DScene* pScene)
: CComponent(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CGeometryGenerator::~CGeometryGenerator()
{
}

//-------------------------------------------------------------------------------------------------

void CGeometryGenerator::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent, CXMLNode xFunctions)
{
}
