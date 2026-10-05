
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CWheel.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CWheel::instantiator(C3DScene* pScene)
{
    return new CWheel(pScene);
}

//-------------------------------------------------------------------------------------------------

CWheel::CWheel(C3DScene* pScene)
: CPhysicalComponent(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CWheel::~CWheel()
{
}

//-------------------------------------------------------------------------------------------------

void CWheel::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CWheel::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);

    QSP<CComponent> pRoot = root();

    if (pRoot != nullptr)
    {
    }
}
