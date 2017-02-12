
// Quick3D
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CAtmosphere.h"
#include "CAircraft.h"

#include "CAirbusStaticPort.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusStaticPort::instanciator(C3DScene* pScene)
{
    return new CAirbusStaticPort(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusStaticPort::CAirbusStaticPort(C3DScene* pScene)
    : CComponent(pScene)
{
    LOG_DEBUG("CAirbusStaticPort::CAirbusStaticPort()");
}

//-------------------------------------------------------------------------------------------------

CAirbusStaticPort::~CAirbusStaticPort()
{
    LOG_DEBUG("CAirbusStaticPort::~CAirbusStaticPort()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusStaticPort::update(double dDeltaTime)
{
    QSP<CAircraft> pAircraft = QSP_CAST(CAircraft, getRoot());

    if (pAircraft)
    {
        double dPressure = CAtmosphere::getInstance()->pressure_Nm2(pAircraft->getGeoloc().Altitude) * N_TO_KG;

        pushData(CAirbusData(m_sName, adStatic_Pressure_kgm2, dPressure));

        LOG_VALUE(QString("%1 (PRS KGM2)").arg(m_sName),
                  QString("%1")
                  .arg(QString::number(dPressure, 'f', 4))
                  );
    }
}
