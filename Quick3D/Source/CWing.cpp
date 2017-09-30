
// qt-plus
#include "CLogger.h"

// Application
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CWing.h"
#include "CAircraft.h"
#include "CAtmosphere.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CWing::instantiator(C3DScene* pScene)
{
    return new CWing(pScene);
}

//-------------------------------------------------------------------------------------------------

CWing::CWing(C3DScene* pScene)
    : CPhysicalComponent(pScene)
    , m_dArea_m2(80.0)
    , m_dWingAngle_rad(-Math::Pi / 18.0)
    , m_dAileronArea_m2(5.0)
    , m_dAileronMaxPositiveAngle_rad(Math::Angles::toRad(-10.0))
    , m_dAileronAngle_rad(0.0)
    , m_dFlapsPosition_norm(0.0)
{
    m_iBodyAirflowDotLiftFactor.addValue(-1.00, 0.0);
    m_iBodyAirflowDotLiftFactor.addValue( 0.00, 0.0);
    m_iBodyAirflowDotLiftFactor.addValue( 0.05, 0.5);
    m_iBodyAirflowDotLiftFactor.addValue( 0.15, 1.0);
    m_iBodyAirflowDotLiftFactor.addValue( 0.30, 0.0);
    m_iBodyAirflowDotLiftFactor.addValue( 1.00, 0.0);

    m_iBodyAirflowDotAileronLiftFactor.addValue(-1.00, 0.0);
    m_iBodyAirflowDotAileronLiftFactor.addValue( 0.00, 1.0);
    m_iBodyAirflowDotAileronLiftFactor.addValue( 1.00, 0.0);
}

//-------------------------------------------------------------------------------------------------

CWing::~CWing()
{
}

//-------------------------------------------------------------------------------------------------

void CWing::setAileronAngle_norm(double dValue)
{
    m_dAileronAngle_rad = Math::Angles::clipDouble(dValue, -1.0, 1.0) * m_dAileronMaxPositiveAngle_rad;
}

//-------------------------------------------------------------------------------------------------

void CWing::setAileronAngle_rad(double dAngle)
{
    m_dAileronAngle_rad = dAngle;
}

//-------------------------------------------------------------------------------------------------

void CWing::setFlapsPosition_norm(double dValue)
{
    m_dFlapsPosition_norm = Math::Angles::clipDouble(dValue, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

void CWing::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xPhysicsNode = xComponent.getNodeByTagName(ParamName_Physics);
    CXMLNode xAileronPositionNode = xComponent.getNodeByTagName(ParamName_AileronPosition);

    if (xPhysicsNode.attributes()["AreaM2"].isEmpty() == false)
    {
        m_dArea_m2 = xPhysicsNode.attributes()["AreaM2"].toDouble();
    }

    if (xPhysicsNode.attributes()["Angle"].isEmpty() == false)
    {
        m_dWingAngle_rad = Math::Angles::toRad(xPhysicsNode.attributes()["Angle"].toDouble());
    }

    if (xPhysicsNode.attributes()["AileronAreaM2"].isEmpty() == false)
    {
        m_dAileronArea_m2 = xPhysicsNode.attributes()["AileronAreaM2"].toDouble();
    }

    if (xPhysicsNode.attributes()["AileronMaxPositiveAngle"].isEmpty() == false)
    {
        m_dAileronMaxPositiveAngle_rad = Math::Angles::toRad(xPhysicsNode.attributes()["AileronMaxPositiveAngle"].toDouble());
    }

    if (xAileronPositionNode.isEmpty() == false)
    {
        m_vAileronPosition = CVector3(
                    xAileronPositionNode.attributes()[ParamName_x].toDouble(),
                    xAileronPositionNode.attributes()[ParamName_y].toDouble(),
                    xAileronPositionNode.attributes()[ParamName_z].toDouble()
                    );
    }
}

//-------------------------------------------------------------------------------------------------

void CWing::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);

    QSP<CComponent> pRoot = root();
    QSP<CAircraft> pAircraft = QSP_CAST(CAircraft, pRoot);

    if (pAircraft != nullptr)
    {
        // Compute lift

        CAxis aRotationAxis;

        aRotationAxis = aRotationAxis.rotate(CVector3(m_dWingAngle_rad, 0.0, 0.0));
        aRotationAxis = aRotationAxis.rotate(pAircraft->rotation());

        CAxis aVelocityAxis(eulerAngles(pAircraft->velocity_ms()));

        aVelocityAxis = aVelocityAxis.transferFrom(aRotationAxis);

        double dDotBodyAirflow = aVelocityAxis.eulerAngles().X / Math::Pi;
        double dDotBodyAirflowCorrected = m_iBodyAirflowDotLiftFactor.getValue(dDotBodyAirflow);
        double dDotBodyAirflowAileronCorrected = m_iBodyAirflowDotAileronLiftFactor.getValue(dDotBodyAirflow);

        double dAirDragFactor = CAtmosphere::getInstance()->airDragFactor(pAircraft->geoloc().Altitude);
        double dVelocity = pAircraft->velocity_ms().magnitude();
        double dTotalMass_kg = pAircraft->totalMass_kg();
        double dMassMultiplier = sqrt(dTotalMass_kg);					// 245
        double dAreaOnMass = m_dArea_m2 / dMassMultiplier;				// 0.32
        double dLift = dVelocity * dAreaOnMass;							// 200 kts = 32

        dLift *= dTotalMass_kg / 100.0;
        dLift += ((dLift * 0.25) * m_dFlapsPosition_norm);
        dLift *= dAirDragFactor;
        dLift *= dDotBodyAirflowCorrected;

        LOG_VALUE(QString("%1 L KG / FLP / DOT").arg(m_sName),
                  QString("%1 / %2 / %3")
                  .arg(QString::number(dLift, 'f', 2))
                  .arg(QString::number(m_dFlapsPosition_norm, 'f', 2))
                  .arg(QString::number(dDotBodyAirflow, 'f', 4))
                  );

        // Apply lift

        pAircraft->addUncenteredLocalForce_kg(CVector3(), CVector3(0.0, dLift, 0.0));

        // Apply aileron lift

        double dAileronAreaOnMass = m_dAileronArea_m2 / dMassMultiplier;
        double dAileronLift = dVelocity * dAileronAreaOnMass;
        dAileronLift *= dTotalMass_kg;
        dAileronLift *= m_dAileronAngle_rad;
        dAileronLift *= 5.0;
        dAileronLift *= dAirDragFactor;
        dAileronLift *= dDotBodyAirflowAileronCorrected;

        CVector3 vAileronForce = CVector3(0.0, 1.0, 0.0) * dAileronLift;
        CVector3 vAileronPosition = position() + m_vAileronPosition;

        pAircraft->addUncenteredLocalForce_kg(vAileronPosition, vAileronForce);

        LOG_VALUE(QString("%1 A POS / A L KG").arg(m_sName),
                  QString("%1 / %2")
                  .arg(QString::number(m_dAileronAngle_rad, 'f', 2))
                  .arg(QString::number(dAileronLift, 'f', 2))
                  );
    }
}

//-------------------------------------------------------------------------------------------------

void CWing::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CWing]"));
    dumpIdent(stream, iIdent, QString("Area m2 : %1").arg(m_dArea_m2));
    dumpIdent(stream, iIdent, QString("Wing angle rad : %1").arg(m_dWingAngle_rad));
    dumpIdent(stream, iIdent, QString("Aileron area m2 : %1").arg(m_dAileronArea_m2));
    dumpIdent(stream, iIdent, QString("Aileron max pos angle rad : %1").arg(m_dAileronMaxPositiveAngle_rad));
    dumpIdent(stream, iIdent, QString("Aileron angle rad : %1").arg(m_dAileronAngle_rad));
    dumpIdent(stream, iIdent, QString("Flaps position norm : %1").arg(m_dFlapsPosition_norm));
    dumpIdent(stream, iIdent, QString("Aileron position : %1").arg(m_vAileronPosition.toString()));

    CPhysicalComponent::dump(stream, iIdent);
}
