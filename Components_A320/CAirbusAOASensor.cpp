
// Quick3D
#include "../Quick3D/CLogManager.h"
#include "../Quick3D/CConsoleBoard.h"
#include "../Quick3D/C3DScene.h"
#include "../Quick3D/CAircraft.h"

// Application
#include "CAirbusAOASensor.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusAOASensor::instanciator(C3DScene* pScene)
{
	return new CAirbusAOASensor(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusAOASensor::CAirbusAOASensor(C3DScene* pScene)
: CComponent(pScene)
{
	LOG_DEBUG("CAirbusAOASensor::CAirbusAOASensor()");
}

//-------------------------------------------------------------------------------------------------

CAirbusAOASensor::~CAirbusAOASensor()
{
	LOG_DEBUG("CAirbusAOASensor::~CAirbusAOASensor()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusAOASensor::update(double dDeltaTime)
{
	CAircraft* pAircraft = dynamic_cast<CAircraft*>(getRoot());

	if (pAircraft != NULL)
	{
		double dAngleOfAttack = pAircraft->getAngleOfAttack_rad();

		pushData(CAirbusData(m_sName, adAir_AngleOfAttack_rad, dAngleOfAttack));

		LOG_VALUE(QString("%1 (AOA)").arg(m_sName),
			QString("%1")
			.arg(QString::number(dAngleOfAttack, 'f', 2))
			);
	}
}
