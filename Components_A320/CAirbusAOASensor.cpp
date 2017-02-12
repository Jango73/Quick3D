
// Quick3D
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CAircraft.h"

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
    QSP<CAircraft> pAircraft = QSP_CAST(CAircraft, getRoot());

    if (pAircraft)
    {
        double dAngleOfAttack = pAircraft->angleOfAttack_rad();

        pushData(CAirbusData(m_sName, adAir_AngleOfAttack_rad, dAngleOfAttack));

        LOG_VALUE(QString("%1 (AOA)").arg(m_sName),
                  QString("%1")
                  .arg(QString::number(dAngleOfAttack, 'f', 2))
                  );
    }
}
