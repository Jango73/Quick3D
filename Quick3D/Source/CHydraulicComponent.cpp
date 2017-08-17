
// Foundations
#include "CLogger.h"

// Application
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CHydraulicComponent.h"

//-------------------------------------------------------------------------------------------------

CComponent* CHydraulicComponent::instanciator(C3DScene* pScene)
{
    return new CHydraulicComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

CHydraulicComponent::CHydraulicComponent(C3DScene* pScene)
    : CComponent(pScene)
    , m_dPressure(0.0)
{
    LOG_DEBUG("CHydraulicComponent::CHydraulicComponent()");
}

//-------------------------------------------------------------------------------------------------

CHydraulicComponent::~CHydraulicComponent()
{
    LOG_DEBUG("CHydraulicComponent::~CHydraulicComponent()");
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);

    QVector<CXMLNode> vInputsNodes = xComponent.getNodesByTagName(ParamName_PowerInput);

    foreach (CXMLNode xInput, vInputsNodes)
    {
        if (xInput.attributes()[ParamName_Name].isEmpty() == false)
        {
            m_vInputs.append(CComponentReference<CHydraulicComponent>(xInput.attributes()[ParamName_Name]));
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::solveLinks(C3DScene* pScene)
{
    CComponent::solveLinks(pScene);

    for (int iIndex = 0; iIndex < m_vInputs.count(); iIndex++)
    {
        m_vInputs[iIndex].solve(pScene, QSP<CComponent>(this));
    }
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::clearLinks(C3DScene* pScene)
{
    CComponent::clearLinks(pScene);

    for (int iIndex = 0; iIndex < m_vInputs.count(); iIndex++)
    {
        m_vInputs[iIndex].clear();
    }

    for (int iIndex = 0; iIndex < m_vOutputs.count(); iIndex++)
    {
        m_vOutputs[iIndex].clear();
    }
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::update(double dDeltaTime)
{
    if (m_vInputs.count() > 0)
    {
        QSP<CHydraulicComponent> pHydraulic = QSP_CAST(CHydraulicComponent, m_vInputs[0].component());

        if (pHydraulic != nullptr)
        {
            double dPressure = pHydraulic->pull(0.0, dDeltaTime);
            push(dPressure, dDeltaTime);
        }
    }

    /*
    LOG_VALUE(QString("%1 (I/U/T)").arg(m_sName),
        QString("%1 %2 %3")
        .arg(QString::number(m_tLoad.m_dAmperage, 'f', 2))
        .arg(QString::number(m_tLoad.m_dVoltage, 'f', 0))
        .arg(QString::number(m_tLoad.m_eType))
        );
    */
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::push(double dPressure, double dDeltaTime)
{
    m_dPressure += dPressure;
}

//-------------------------------------------------------------------------------------------------

double CHydraulicComponent::pull(double dPressure, double dDeltaTime)
{
    m_dPressure -= dPressure * dDeltaTime;

    return m_dPressure;
}
