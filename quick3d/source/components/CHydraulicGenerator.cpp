
// qt-plus
#include "CLogger.h"

// Quick3D
#include "C3DScene.h"

// Application
#include "CHydraulicGenerator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CHydraulicGenerator::instantiator(C3DScene* pScene)
{
    return new CHydraulicGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CHydraulicGenerator::CHydraulicGenerator(C3DScene* pScene)
    : CGenerator(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CHydraulicGenerator::~CHydraulicGenerator()
{
}

//-------------------------------------------------------------------------------------------------

void CHydraulicGenerator::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CGenerator::loadParameters(sBaseFile, xComponent);

    CXMLNode xNode = xComponent.getNodeByTagName(ParamName_HydraulicInput);

    m_rHydraulicInput.setName(xNode.attributes()[ParamName_Name]);
}

//-------------------------------------------------------------------------------------------------

void CHydraulicGenerator::solveLinks(C3DScene* pScene)
{
    CGenerator::solveLinks(pScene);

    m_rHydraulicInput.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CHydraulicGenerator::clearLinks(C3DScene* pScene)
{
    CGenerator::clearLinks(pScene);

    m_rHydraulicInput.clear();
}

//-------------------------------------------------------------------------------------------------

void CHydraulicGenerator::update(double dDeltaTime)
{
    CElectricalComponent::update(dDeltaTime);

    QSP<CHydraulicComponent> pInput = QSP_CAST(CHydraulicComponent, m_rHydraulicInput.component());

    if (pInput != nullptr && pInput->pressure_norm() > 0.5)
    {
        push(m_tCurrent, dDeltaTime);
    }
    else
    {
        push(CElectricalLoad::noPower(), dDeltaTime);
    }
}
