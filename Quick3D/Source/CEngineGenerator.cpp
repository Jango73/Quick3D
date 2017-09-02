
// qt-plus
#include "CLogger.h"

// Quick3D
#include "C3DScene.h"

// Application
#include "CEngineGenerator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CEngineGenerator::instanciator(C3DScene* pScene)
{
    return new CEngineGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CEngineGenerator::CEngineGenerator(C3DScene* pScene)
    : CElectricalComponent(pScene)
    , m_tCurrent(ctAC, 200.0, 150.0, 400.0)
{
    LOG_DEBUG("CEngineGenerator::CEngineGenerator()");
}

//-------------------------------------------------------------------------------------------------

CEngineGenerator::~CEngineGenerator()
{
    LOG_DEBUG("CEngineGenerator::~CEngineGenerator()");
}

//-------------------------------------------------------------------------------------------------

CElectricalLoad& CEngineGenerator::current()
{
    return m_tCurrent;
}

//-------------------------------------------------------------------------------------------------

const CElectricalLoad& CEngineGenerator::current() const
{
    return m_tCurrent;
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CElectricalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xNode = xComponent.getNodeByTagName(ParamName_EngineInput);

    m_rEngineTarget.setName(xNode.attributes()[ParamName_Name]);
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::solveLinks(C3DScene* pScene)
{
    CElectricalComponent::solveLinks(pScene);

    m_rEngineTarget.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::clearLinks(C3DScene* pScene)
{
    CElectricalComponent::clearLinks(pScene);

    m_rEngineTarget.clear();
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::update(double dDeltaTime)
{
    CElectricalComponent::update(dDeltaTime);

    QSP<CEngine> pEngine = QSP_CAST(CEngine, m_rEngineTarget.component());

    if (pEngine != nullptr && pEngine->alternatorActive())
    {
        push(m_tCurrent, dDeltaTime);
    }
    else
    {
        push(CElectricalLoad::noPower(), dDeltaTime);
    }
}
