
// Application
#include "C3DScene.h"
#include "CEngine.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CEngine::instantiator(C3DScene* pScene)
{
    return new CEngine(pScene);
}

//-------------------------------------------------------------------------------------------------

CEngine::CEngine(C3DScene* pScene)
    : CPhysicalComponent(pScene)
    , m_dMaxThrust_kg(100.0)
    , m_dFuelFlow_norm(0.0)
{
}

//-------------------------------------------------------------------------------------------------

CEngine::~CEngine()
{
}

//-------------------------------------------------------------------------------------------------

void CEngine::setCurrentFuelFlow_norm(double value)
{
    m_dFuelFlow_norm = Math::Angles::clipDouble(value, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

double CEngine::currentFuelFlow_norm() const
{
    return m_dFuelFlow_norm;
}

//-------------------------------------------------------------------------------------------------

double CEngine::currentThrust_kg() const
{
    return m_dFuelFlow_norm * m_dMaxThrust_kg;
}

//-------------------------------------------------------------------------------------------------

double CEngine::currentFuelCons_ls() const
{
    return m_dFuelFlow_norm * 0.01;
}

//-------------------------------------------------------------------------------------------------

bool CEngine::alternatorActive() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------

void CEngine::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xThrustNode = xComponent.getNodeByTagName(ParamName_Thrust);

    if (xThrustNode.attributes()[ParamName_MaxThrustKG].isEmpty() == false)
    {
        m_dMaxThrust_kg = xThrustNode.attributes()[ParamName_MaxThrustKG].toDouble();
    }
}

//-------------------------------------------------------------------------------------------------

void CEngine::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);

    QSP<CComponent> pRoot = root();
    QSP<CPhysicalComponent> pPhysical = QSP_CAST(CPhysicalComponent, pRoot);

    if (pPhysical != nullptr)
    {
        double dAirFactor = CAtmosphere::getInstance()->airDragFactor(pPhysical->geoloc().Altitude);
        dAirFactor = Math::Angles::clipDouble((dAirFactor * 2.0) * 40.0, 0.0, 40.0);
        double dCurrentThrust_kg = currentThrust_kg() * dAirFactor;

        pPhysical->addUncenteredLocalForce_kg(position(), CAxis(rotation()).Front * dCurrentThrust_kg);
    }
}
