
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CWheel.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CWheel::instantiator(C3DScene* pScene)
{
    return new CWheel(pScene);
}

//-------------------------------------------------------------------------------------------------

CWheel::CWheel(C3DScene* pScene)
: CPhysicalComponent(pScene)
, m_dRadiusM(0.4)
, m_dWidthM(0.2)
, m_dMaxSteerAngleDeg(30.0)
, m_dSuspensionMinLengthM(0.35)
, m_dSuspensionMaxLengthM(0.65)
, m_dSuspensionFrequencyHz(1.5)
, m_dSuspensionDampingNorm(0.5)
, m_dLongitudinalFrictionNorm(1.0)
, m_dLateralFrictionNorm(1.0)
, m_bDriven(false)
, m_bSteered(false)
, m_bHandBrake(false)
{
}

//-------------------------------------------------------------------------------------------------

CWheel::~CWheel()
{
}

//-------------------------------------------------------------------------------------------------

void CWheel::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

    if (xGeneralNode.isEmpty() == false)
    {
        if (xGeneralNode.attributes()["Radius"].isEmpty() == false)
        {
            m_dRadiusM = xGeneralNode.attributes()["Radius"].toDouble();
        }

        if (xGeneralNode.attributes()["Width"].isEmpty() == false)
        {
            m_dWidthM = xGeneralNode.attributes()["Width"].toDouble();
        }

        if (xGeneralNode.attributes()["Driven"].isEmpty() == false)
        {
            m_bDriven = xGeneralNode.attributes()["Driven"].toInt() != 0;
        }

        if (xGeneralNode.attributes()["Steered"].isEmpty() == false)
        {
            m_bSteered = xGeneralNode.attributes()["Steered"].toInt() != 0;
        }

        if (xGeneralNode.attributes()["HandBrake"].isEmpty() == false)
        {
            m_bHandBrake = xGeneralNode.attributes()["HandBrake"].toInt() != 0;
        }

        if (xGeneralNode.attributes()["MaxSteerAngleDeg"].isEmpty() == false)
        {
            m_dMaxSteerAngleDeg = xGeneralNode.attributes()["MaxSteerAngleDeg"].toDouble();
        }

        if (xGeneralNode.attributes()["SuspensionMinLength"].isEmpty() == false)
        {
            m_dSuspensionMinLengthM = xGeneralNode.attributes()["SuspensionMinLength"].toDouble();
        }

        if (xGeneralNode.attributes()["SuspensionMaxLength"].isEmpty() == false)
        {
            m_dSuspensionMaxLengthM = xGeneralNode.attributes()["SuspensionMaxLength"].toDouble();
        }

        if (xGeneralNode.attributes()["SuspensionFrequencyHz"].isEmpty() == false)
        {
            m_dSuspensionFrequencyHz = xGeneralNode.attributes()["SuspensionFrequencyHz"].toDouble();
        }

        if (xGeneralNode.attributes()["SuspensionDamping"].isEmpty() == false)
        {
            m_dSuspensionDampingNorm = xGeneralNode.attributes()["SuspensionDamping"].toDouble();
        }

        if (xGeneralNode.attributes()["LongitudinalFriction"].isEmpty() == false)
        {
            m_dLongitudinalFrictionNorm = xGeneralNode.attributes()["LongitudinalFriction"].toDouble();
        }

        if (xGeneralNode.attributes()["LateralFriction"].isEmpty() == false)
        {
            m_dLateralFrictionNorm = xGeneralNode.attributes()["LateralFriction"].toDouble();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWheel::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);

    QSP<CComponent> pRoot = root();

    if (pRoot != nullptr)
    {
    }
}
