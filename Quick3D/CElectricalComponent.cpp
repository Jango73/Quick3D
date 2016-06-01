
// Application
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CElectricalComponent.h"

//-------------------------------------------------------------------------------------------------

CComponent* CElectricalComponent::instanciator(C3DScene* pScene)
{
	return new CElectricalComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

CElectricalComponent::CElectricalComponent(C3DScene* pScene)
: CComponent(pScene)
, m_dMaxAmperage(150.0)
, m_bShortCircuited(false)
{
	LOG_DEBUG("CElectricalComponent::CElectricalComponent()");
}

//-------------------------------------------------------------------------------------------------

CElectricalComponent::~CElectricalComponent()
{
	LOG_DEBUG("CElectricalComponent::~CElectricalComponent()");
}

//-------------------------------------------------------------------------------------------------

void CElectricalComponent::loadParameters(CXMLNode xComponent)
{
	CComponent::loadParameters(xComponent);

	QVector<CXMLNode> vInputsNodes = xComponent.getNodesByTagName(ParamName_PowerInput);

	foreach (CXMLNode xInput, vInputsNodes)
	{
		if (xInput.m_vAttributes[ParamName_Name].isEmpty() == false)
		{
			m_vPowerInputNames.append(xInput.m_vAttributes[ParamName_Name]);
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CElectricalComponent::solveLinks(C3DScene* pScene)
{
	CComponent::solveLinks(pScene);

	foreach (QString sName, m_vPowerInputNames)
	{
		foreach (QSharedPointer<CComponent> pComponent, pScene->getComponents())
		{
			CComponent* pFound = pComponent->findComponent(sName, this);

			if (pFound != NULL)
			{
				CElectricalComponent* pInput = dynamic_cast<CElectricalComponent*>(pFound);

				if (pInput != NULL)
				{
					m_vPowerInputs.append(pInput);
					pInput->m_vPowerOutputs.append(this);
				}

				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CElectricalComponent::update(double dDeltaTime)
{
	if (m_vPowerInputs.count() > 0)
	{
		CElectricalLoad tLoad = m_vPowerInputs[0]->pull(0.0, dDeltaTime);
		push(tLoad, dDeltaTime);
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

void CElectricalComponent::push(const CElectricalLoad& tLoad, double dDeltaTime)
{
	m_tLoad.m_eType = tLoad.m_eType;
	m_tLoad.m_dFrequency = tLoad.m_dFrequency;
	m_tLoad.m_dVoltage = tLoad.m_dVoltage;
	m_tLoad.m_dAmperage += tLoad.m_dAmperage;

	if (m_tLoad.m_dAmperage > m_dMaxAmperage)
	{
		m_tLoad.m_dAmperage = m_dMaxAmperage;
	}
}

//-------------------------------------------------------------------------------------------------

CElectricalLoad CElectricalComponent::pull(double dAmperage, double dDeltaTime)
{
	double dElecDeltaTime = dDeltaTime * 2.0;
	CElectricalLoad tReturnedLoad = m_tLoad;
	tReturnedLoad.m_dAmperage = 0.0;

	if (dAmperage == 0.0)
	{
		dAmperage = m_tLoad.m_dAmperage;
	}

	tReturnedLoad.m_dAmperage += dAmperage * dElecDeltaTime;
	if (tReturnedLoad.m_dAmperage > m_tLoad.m_dAmperage)
	{
		tReturnedLoad.m_dAmperage = m_tLoad.m_dAmperage;
	}
	m_tLoad.m_dAmperage -= dAmperage * dElecDeltaTime;

	if (m_tLoad.m_dAmperage < 0.0)
	{
		m_tLoad.m_dAmperage = 0.0;
	}

	return tReturnedLoad;
}
