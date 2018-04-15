
// qt-plus
#include "CLogger.h"

// Quick3D
#include "C3DScene.h"

// Application
#include "CEngineGenerator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CEngineGenerator::instantiator(C3DScene* pScene)
{
    return new CEngineGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CEngineGenerator::CEngineGenerator(C3DScene* pScene)
    : CGenerator(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CEngineGenerator::~CEngineGenerator()
{
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CGenerator::loadParameters(sBaseFile, xComponent);

    CXMLNode xNode = xComponent.getNodeByTagName(ParamName_EngineInput);

    m_rEngineInput.setName(xNode.attributes()[ParamName_Name]);
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::solveLinks(C3DScene* pScene)
{
    CGenerator::solveLinks(pScene);

    m_rEngineInput.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::clearLinks(C3DScene* pScene)
{
    CGenerator::clearLinks(pScene);

    m_rEngineInput.clear();
}

//-------------------------------------------------------------------------------------------------

void CEngineGenerator::update(double dDeltaTime)
{
    CGenerator::update(dDeltaTime);

    QSP<CEngine> pEngine = QSP_CAST(CEngine, m_rEngineInput.component());

    if (pEngine != nullptr && pEngine->alternatorActive())
    {
        push(m_tCurrent, dDeltaTime);
    }
    else
    {
        push(CElectricalLoad::noPower(), dDeltaTime);
    }
}
