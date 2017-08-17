
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CWheel.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CWheel::instanciator(C3DScene* pScene)
{
    return new CWheel(pScene);
}

//-------------------------------------------------------------------------------------------------

CWheel::CWheel(C3DScene* pScene)
: CPhysicalComponent(pScene)
{
    LOG_DEBUG("CWheel::CWheel()");
}

//-------------------------------------------------------------------------------------------------

CWheel::~CWheel()
{
    LOG_DEBUG("CWheel::~CWheel()");
}

//-------------------------------------------------------------------------------------------------

void CWheel::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CWheel::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);
}
