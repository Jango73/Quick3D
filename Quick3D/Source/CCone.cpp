
// Application
#include "CCone.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CCone::CCone(
        C3DScene* pScene,
        double dMaxDistance,
        int iNumSegments,
        int iNumHeightSegments,
        double dHeight,
        double dBaseRadius,
        double dApexRadius
        )
    : CMesh(pScene, dMaxDistance)
{
    setName("Cone");

    m_pGeometry->setMaterial(m_pScene->getRessourcesManager()->getDefaultMaterial());
    m_pGeometry->getMaterials()[0]->setIRFactor(0.4);

    m_pGeometry->createCone(iNumSegments, iNumHeightSegments, dHeight, dBaseRadius, dApexRadius);
}

//-------------------------------------------------------------------------------------------------

CCone::~CCone()
{
}
