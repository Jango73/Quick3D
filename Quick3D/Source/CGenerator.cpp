
// qt-plus
#include "CLogger.h"

// Quick3D
#include "C3DScene.h"

// Application
#include "CGenerator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CGenerator::instantiator(C3DScene* pScene)
{
    return new CGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CGenerator::CGenerator(C3DScene* pScene)
    : CElectricalComponent(pScene)
    , m_tCurrent(ctAC, 200.0, 150.0, 400.0)
{
}

//-------------------------------------------------------------------------------------------------

CGenerator::~CGenerator()
{
}

//-------------------------------------------------------------------------------------------------

CElectricalLoad& CGenerator::current()
{
    return m_tCurrent;
}

//-------------------------------------------------------------------------------------------------

const CElectricalLoad& CGenerator::current() const
{
    return m_tCurrent;
}

//-------------------------------------------------------------------------------------------------

void CGenerator::update(double dDeltaTime)
{
    CElectricalComponent::update(dDeltaTime);
}
