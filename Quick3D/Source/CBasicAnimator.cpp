
// Fondations
#include "CLogManager.h"

// Application
#include "C3DScene.h"
#include "CBasicAnimator.h"
#include "CBasicAnimatorFrame.h"
#include "CServoAnimatorFrame.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CBasicAnimator::instanciator(C3DScene* pScene)
{
    return new CBasicAnimator(pScene);
}

//-------------------------------------------------------------------------------------------------

CBasicAnimator::CBasicAnimator(C3DScene* pScene)
    : CAnimator(pScene)
{
    LOG_DEBUG("CBasicAnimator::CBasicAnimator()");

    m_pTranslationFrame = NULL;
    m_pRotationFrame = NULL;
}

//-------------------------------------------------------------------------------------------------

CBasicAnimator::~CBasicAnimator()
{
    LOG_DEBUG("CBasicAnimator::CBasicAnimator()");

    if (m_pTranslationFrame != NULL)
    {
        delete m_pTranslationFrame;
    }

    if (m_pRotationFrame != NULL)
    {
        delete m_pRotationFrame;
    }
}

//-------------------------------------------------------------------------------------------------

void CBasicAnimator::addTranslationStep(CBasicAnimationStep value)
{
    if (m_pTranslationFrame != NULL) m_pTranslationFrame->addStep(value);
}

//-------------------------------------------------------------------------------------------------

void CBasicAnimator::addRotationStep(CBasicAnimationStep value)
{
    if (m_pRotationFrame != NULL) m_pRotationFrame->addStep(value);
}

//-------------------------------------------------------------------------------------------------

void CBasicAnimator::loadParameters(CXMLNode xNode)
{
    CAnimator::loadParameters(xNode);

    CXMLNode xGeneralNode = xNode.getNodeByTagName(ParamName_General);

    if (xGeneralNode.isEmpty() == false)
    {
        if (xGeneralNode.attributes()[ParamName_Type].isEmpty() == false)
        {
            if (xGeneralNode.attributes()[ParamName_Type] == ParamName_Servo)
            {
                m_pTranslationFrame = new CServoAnimatorFrame();
                m_pRotationFrame = new CServoAnimatorFrame();

                if (xGeneralNode.attributes()[ParamName_AccelerationFactor].isEmpty() == false)
                {
                    double dAccelerationFactor = xGeneralNode.attributes()[ParamName_AccelerationFactor].toDouble();

                    ((CServoAnimatorFrame*)m_pTranslationFrame)->setAccelerationFactor(dAccelerationFactor);
                    ((CServoAnimatorFrame*)m_pRotationFrame)->setAccelerationFactor(dAccelerationFactor);
                }

                if (xGeneralNode.attributes()[ParamName_VelocityFactor].isEmpty() == false)
                {
                    double dVelocityFactor = xGeneralNode.attributes()[ParamName_VelocityFactor].toDouble();

                    ((CServoAnimatorFrame*)m_pTranslationFrame)->setVelocityFactor(dVelocityFactor);
                    ((CServoAnimatorFrame*)m_pRotationFrame)->setVelocityFactor(dVelocityFactor);
                }
            }
        }
    }

    if (m_pTranslationFrame == NULL)
    {
        m_pTranslationFrame = new CBasicAnimatorFrame();
    }

    if (m_pRotationFrame == NULL)
    {
        m_pRotationFrame = new CBasicAnimatorFrame();
    }

    {
        CXMLNode xStepsNode = xNode.getNodeByTagName(ParamName_Translation);

        if (xStepsNode.isEmpty() == false)
        {
            QVector<CXMLNode> lStepNodes = xStepsNode.getNodesByTagName(ParamName_Step);

            foreach (CXMLNode xStep, lStepNodes)
            {
                CXMLNode xTargetNode = xStep.getNodeByTagName(ParamName_Target);
                CXMLNode xSpeedNode = xStep.getNodeByTagName(ParamName_Velocity);
                CXMLNode xAccelerationNode = xStep.getNodeByTagName(ParamName_Acceleration);
                double dDuration = xStep.attributes()[ParamName_Pause].toDouble();

                CVector3 vTarget = CVector3(
                            xTargetNode.attributes()[ParamName_x].toDouble(),
                            xTargetNode.attributes()[ParamName_y].toDouble(),
                            xTargetNode.attributes()[ParamName_z].toDouble()
                            );

                CVector3 vSpeed = CVector3(
                            xSpeedNode.attributes()[ParamName_x].toDouble(),
                            xSpeedNode.attributes()[ParamName_y].toDouble(),
                            xSpeedNode.attributes()[ParamName_z].toDouble()
                            );

                CVector3 vAcceleration = CVector3(
                            xAccelerationNode.attributes()[ParamName_x].toDouble(),
                            xAccelerationNode.attributes()[ParamName_y].toDouble(),
                            xAccelerationNode.attributes()[ParamName_z].toDouble()
                            );

                addTranslationStep(CBasicAnimationStep(vTarget, vSpeed, vAcceleration, dDuration));
            }
        }
    }

    {
        CXMLNode xStepsNode = xNode.getNodeByTagName("Rotation");

        if (xStepsNode.isEmpty() == false)
        {
            QVector<CXMLNode> lStepNodes = xStepsNode.getNodesByTagName("Step");

            foreach (CXMLNode xStep, lStepNodes)
            {
                CXMLNode xTargetNode = xStep.getNodeByTagName("Target");
                CXMLNode xSpeedNode = xStep.getNodeByTagName("Speed");
                CXMLNode xAccelerationNode = xStep.getNodeByTagName("Acceleration");
                double dDuration = xStep.attributes()["Pause"].toDouble();

                CVector3 vTarget = CVector3(
                            Math::Angles::toRad(xTargetNode.attributes()[ParamName_x].toDouble()),
                            Math::Angles::toRad(xTargetNode.attributes()[ParamName_y].toDouble()),
                            Math::Angles::toRad(xTargetNode.attributes()[ParamName_z].toDouble())
                            );

                CVector3 vSpeed = CVector3(
                            Math::Angles::toRad(xSpeedNode.attributes()[ParamName_x].toDouble()),
                            Math::Angles::toRad(xSpeedNode.attributes()[ParamName_y].toDouble()),
                            Math::Angles::toRad(xSpeedNode.attributes()[ParamName_z].toDouble())
                            );

                CVector3 vAcceleration = CVector3(
                            Math::Angles::toRad(xAccelerationNode.attributes()[ParamName_x].toDouble()),
                            Math::Angles::toRad(xAccelerationNode.attributes()[ParamName_y].toDouble()),
                            Math::Angles::toRad(xAccelerationNode.attributes()[ParamName_z].toDouble())
                            );

                addRotationStep(CBasicAnimationStep(vTarget, vSpeed, vAcceleration, dDuration));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CBasicAnimator::update(double dDeltaTime)
{
    if (m_bIsPlaying)
    {
        if (m_rPositionTarget.component() != NULL && m_pTranslationFrame != NULL)
        {
            // m_pPositionTarget->setPosition(m_pTranslationFrame->compute(dDeltaTime, m_pPositionTarget->getPosition()));
            m_rPositionTarget.component()->setOriginPosition(m_pTranslationFrame->compute(dDeltaTime, m_rPositionTarget.component()->getOriginPosition()));
        }

        if (m_rRotationTarget.component() != NULL && m_pRotationFrame != NULL)
        {
            // m_pRotationTarget->setRotation(m_pRotationFrame->compute(dDeltaTime, m_pRotationTarget->getRotation()));
            m_rRotationTarget.component()->setOriginRotation(m_pRotationFrame->compute(dDeltaTime, m_rRotationTarget.component()->getOriginRotation()));
        }
    }
}

//-------------------------------------------------------------------------------------------------
