
// Application
#include "C3DScene.h"
#include "CEngine.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CEngine::instanciator(C3DScene* pScene)
{
	return new CEngine(pScene);
}

//-------------------------------------------------------------------------------------------------

CEngine::CEngine(C3DScene* pScene)
: CPhysicalComponent(pScene)
, m_dMaxThrust_kg(100.0)
, m_dFuelFlow_norm(0.0)
{
	LOG_DEBUG("CEngine::CEngine()");
}

//-------------------------------------------------------------------------------------------------

CEngine::~CEngine()
{
	LOG_DEBUG("CEngine::~CEngine()");
}

//-------------------------------------------------------------------------------------------------

void CEngine::setCurrentFuelFlow_norm(double value)
{
	m_dFuelFlow_norm = Math::Angles::clipDouble(value, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

double CEngine::getCurrentFuelFlow_norm() const
{
	return m_dFuelFlow_norm;
}

//-------------------------------------------------------------------------------------------------

double CEngine::getCurrentThrust_kg() const
{
	return m_dFuelFlow_norm * 100.0;
}

//-------------------------------------------------------------------------------------------------

double CEngine::getCurrentFuelCons_ls() const
{
	return m_dFuelFlow_norm * 0.01;
}

//-------------------------------------------------------------------------------------------------

void CEngine::loadParameters(CXMLNode xComponent)
{
	CPhysicalComponent::loadParameters(xComponent);

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

	CComponent* pRoot = getRoot();
	CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pRoot);

	if (pPhysical != NULL)
	{
		double dAirFactor = CAtmosphere::getInstance()->getAirDragFactor(pPhysical->getGeoloc().Altitude);
		dAirFactor = Math::Angles::clipDouble((dAirFactor * 2.0) * 40.0, 0.0, 40.0);
		double dCurrentThrust_kg = getCurrentThrust_kg() * dAirFactor;

		pPhysical->addUncenteredLocalForce_kg(getOriginPosition(), CVector3(0.0, 0.0, dCurrentThrust_kg));
	}
}
