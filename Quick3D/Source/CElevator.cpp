
// Foundations
#include "CLogger.h"
#include "CConsoleBoard.h"

// Application
#include "C3DScene.h"
#include "CElevator.h"
#include "CAircraft.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CElevator::instanciator(C3DScene* pScene)
{
    return new CElevator(pScene);
}

//-------------------------------------------------------------------------------------------------

CElevator::CElevator(C3DScene* pScene)
    : CWing(pScene)
{
    LOG_DEBUG("CElevator::CElevator()");
}

//-------------------------------------------------------------------------------------------------

CElevator::~CElevator()
{
    LOG_DEBUG("CElevator::~CElevator()");
}

//-------------------------------------------------------------------------------------------------

void CElevator::update(double dDeltaTime)
{
    QSP<CComponent> pRoot = getRoot();
    QSP<CAircraft> pAircraft = QSP_CAST(CAircraft, pRoot);

    if (pAircraft)
    {
        // Apply aileron lift

        CAxis aRotationAxis;

        aRotationAxis = aRotationAxis.rotate(CVector3(m_dWingAngle_rad, 0.0, 0.0));
        aRotationAxis = aRotationAxis.rotate(pAircraft->getOriginRotation());

        CAxis aVelocityAxis(euleurAngles(pAircraft->velocity_ms()));

        aVelocityAxis = aVelocityAxis.transferFrom(aRotationAxis);

        double dDotBodyAirflow = aVelocityAxis.euleurAngles().X / Math::Pi;
        double dDotBodyAirflowAileronCorrected = m_iBodyAirflowDotAileronLiftFactor.getValue(dDotBodyAirflow);

        double dAirDragFactor = CAtmosphere::getInstance()->airDragFactor(pAircraft->getGeoloc().Altitude);
        double dVelocity = pAircraft->velocity_ms().magnitude();
        double dTotalMass_kg = pAircraft->totalMass_kg();
        double dMassMultiplier = sqrt(dTotalMass_kg);
        double dAileronAreaOnMass = m_dAileronArea_m2 / dMassMultiplier;
        double dAileronLift = dVelocity * dAileronAreaOnMass;

        dAileronLift *= dTotalMass_kg;
        dAileronLift *= m_dAileronAngle_rad;
        dAileronLift *= 5.0;
        dAileronLift *= dAirDragFactor;
        dAileronLift *= dDotBodyAirflowAileronCorrected;

        CVector3 vAileronForce = CVector3(0.0, 1.0, 0.0) * dAileronLift;
        CVector3 vAileronPosition = getOriginPosition() + m_vAileronPosition;

        pAircraft->addUncenteredLocalForce_kg(vAileronPosition, vAileronForce);

        LOG_VALUE(QString("%1 AIL POS / AIL LIFT KG").arg(m_sName),
                  QString("%1 / %2")
                  .arg(QString::number(m_dAileronAngle_rad, 'f', 2))
                  .arg(QString::number(dAileronLift, 'f', 2))
                  );
    }
}
