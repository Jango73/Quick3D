
// Quick3D
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CElectricalComponent.h"

// Application
#include "CAirbusFlightComputer.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusFlightComputer::instanciator(C3DScene* pScene)
{
	return new CAirbusFlightComputer(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusFlightComputer::CAirbusFlightComputer(C3DScene* pScene)
: CElectricalConsumer(pScene)
, m_iUnitIndex(0)
{
	LOG_DEBUG("CAirbusFlightComputer::CAirbusFlightComputer()");

	m_tCurrent.m_eType = ctDC;
	m_tCurrent.m_dVoltage = 12.0;
	m_tCurrent.m_dAmperage = 1.0;
	m_tCurrent.m_dFrequency = 100.0;
}

//-------------------------------------------------------------------------------------------------

CAirbusFlightComputer::~CAirbusFlightComputer()
{
	LOG_DEBUG("CAirbusFlightComputer::~CAirbusFlightComputer()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightComputer::loadParameters(CXMLNode xComponent)
{
	CElectricalConsumer::loadParameters(xComponent);
	CAirbusDataSupplier::loadParameters(xComponent);

	CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

	if (xGeneralNode.m_vAttributes[ParamName_UnitIndex].isEmpty() == false)
	{
		m_iUnitIndex = xGeneralNode.m_vAttributes[ParamName_UnitIndex].toInt();
	}
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightComputer::solveLinks(C3DScene* pScene)
{
	CElectricalConsumer::solveLinks(pScene);
	CAirbusDataSupplier::solveLinks(pScene, this);
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightComputer::update(double dDeltaTime)
{
	CElectricalConsumer::update(dDeltaTime);

	LOG_VALUE(QString("%1 (P/S)").arg(m_sName),
		QString("%1 %2")
		.arg(QString::number(m_bPowered))
		.arg(QString::number(m_dStatus, 'f', 2))
		);

	if (m_bPowered)
	{
		work(dDeltaTime);

		sendData();
	}
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightComputer::work(double dDeltaTime)
{
}
