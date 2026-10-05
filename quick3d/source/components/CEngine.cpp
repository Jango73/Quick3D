
// Application
#include "C3DScene.h"
#include "CEngine.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CEngine
    \brief The base class for an engine.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CEngine.
    \a pScene is the scene containing the component.
*/
CComponent* CEngine::instantiator(C3DScene* pScene)
{
    return new CEngine(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CEngine with its default parameters.
    \a pScene is the scene containing the component.
*/
CEngine::CEngine(C3DScene* pScene)
    : CPhysicalComponent(pScene)
    , m_dMaxThrust_kg(100.0)
    , m_dFuelFlow_norm(0.0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CEngine.
*/
CEngine::~CEngine()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the current fuel flow to the normalized \a value.
*/
void CEngine::setCurrentFuelFlow_norm(double value)
{
    m_dFuelFlow_norm = Math::Angles::clipDouble(value, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the normalized current fuel flow.
*/
double CEngine::currentFuelFlow_norm() const
{
    return m_dFuelFlow_norm;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the current thrust in kilograms.
*/
double CEngine::currentThrust_kg() const
{
    return m_dFuelFlow_norm * m_dMaxThrust_kg;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the current fuel consumption in liters per second.
*/
double CEngine::currentFuelCons_ls() const
{
    return m_dFuelFlow_norm * 0.01;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the alternator is active.
*/
bool CEngine::alternatorActive() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CEngine::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xThrustNode = xComponent.getNodeByTagName(ParamName_Thrust);

    if (xThrustNode.attributes()[ParamName_MaxThrustKG].isEmpty() == false)
    {
        m_dMaxThrust_kg = xThrustNode.attributes()[ParamName_MaxThrustKG].toDouble();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CEngine::update(double dDeltaTimeS)
{
    CPhysicalComponent::update(dDeltaTimeS);

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
