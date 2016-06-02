
// Application
#include "CConsoleBoard.h"
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CAircraftController.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAircraftController::instanciator(C3DScene* pScene)
{
	return new CAircraftController(pScene);
}

//-------------------------------------------------------------------------------------------------

CAircraftController::CAircraftController(C3DScene* pScene)
: CStandardController(pScene)
, m_bAileronLeft(false)
, m_bAileronRight(false)
, m_bNoseUp(false)
, m_bNoseDown(false)
, m_bRudderLeft(false)
, m_bRudderRight(false)
{
}

//-------------------------------------------------------------------------------------------------

CAircraftController::~CAircraftController()
{
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::loadParameters(CXMLNode xNode)
{
	CStandardController::loadParameters(xNode);

	m_rLeftWingTarget.setName(xNode.m_vAttributes["LeftWingTarget"]);
	m_rRightWingTarget.setName(xNode.m_vAttributes["RightWingTarget"]);
	m_rElevatorTarget.setName(xNode.m_vAttributes["ElevatorTarget"]);
	m_rRudderTarget.setName(xNode.m_vAttributes["RudderTarget"]);
	m_rEngine1Target.setName(xNode.m_vAttributes["Engine1Target"]);
	m_rEngine2Target.setName(xNode.m_vAttributes["Engine2Target"]);
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::solveLinks(C3DScene* pScene)
{
	CStandardController::solveLinks(pScene);

	m_rLeftWingTarget.solve(pScene, this);
	m_rRightWingTarget.solve(pScene, this);
	m_rElevatorTarget.solve(pScene, this);
	m_rRudderTarget.solve(pScene, this);
	m_rEngine1Target.solve(pScene, this);
	m_rEngine2Target.solve(pScene, this);
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::update(double dDeltaTime)
{
	CController::update(dDeltaTime);

	CWing* pLeftWing = m_rLeftWingTarget.component();
	CWing* pRightWing = m_rRightWingTarget.component();
	CElevator* pElevator = m_rElevatorTarget.component();
	CRudder* pRudder = m_rRudderTarget.component();

	if (m_pJoystick != NULL && m_pJoystick->connected())
	{
		LOG_VALUE(QString("JOY X / Y / Z / R"),
			QString("%1 / %2 / %3 / %4")
			.arg(QString::number(m_pJoystick->getAxisStates()[0], 'f', 2))
			.arg(QString::number(m_pJoystick->getAxisStates()[1], 'f', 2))
			.arg(QString::number(m_pJoystick->getAxisStates()[2], 'f', 2))
			.arg(QString::number(m_pJoystick->getAxisStates()[3], 'f', 2))
			);

		if (pLeftWing != NULL && pRightWing != NULL && pElevator != NULL)
		{
			pLeftWing->setAileronAngle_norm(m_pJoystick->getAxisStates()[0]);
			pRightWing->setAileronAngle_norm(m_pJoystick->getAxisStates()[0] * -1.0);
		}

		if (pElevator != NULL)
		{
			pElevator->setAileronAngle_norm(m_pJoystick->getAxisStates()[1]);
		}

		if (pRudder != NULL)
		{
			pRudder->setAileronAngle_norm(m_pJoystick->getAxisStates()[3]);
		}

		CEngine* pEngine1 = m_rEngine1Target.component();
		CEngine* pEngine2 = m_rEngine2Target.component();

		double dAxis = 1.0 - ((m_pJoystick->getAxisStates()[2] + 1.0) * 0.5);

		if (pEngine1 != NULL)
		{
			pEngine1->setCurrentFuelFlow_norm(dAxis);
		}

		if (pEngine2 != NULL)
		{
			pEngine2->setCurrentFuelFlow_norm(dAxis);
		}
	}
	else
	{
		if (pLeftWing != NULL && pRightWing != NULL)
		{
			if (m_bAileronLeft)
			{
				pLeftWing->setAileronAngle_norm(-1.0);
				pRightWing->setAileronAngle_norm(1.0);
			}
			else
			if (m_bAileronRight)
			{
				pLeftWing->setAileronAngle_norm(1.0);
				pRightWing->setAileronAngle_norm(-1.0);
			}
			else
			{
				pLeftWing->setAileronAngle_norm(0.0);
				pRightWing->setAileronAngle_norm(0.0);
			}
		}

		if (pElevator != NULL)
		{
			if (m_bNoseUp)
			{
				pElevator->setAileronAngle_norm(1.0);
			}
			else
			if (m_bNoseDown)
			{
				pElevator->setAileronAngle_norm(-1.0);
			}
			else
			{
				pElevator->setAileronAngle_norm(0.0);
			}
		}

		if (pRudder != NULL)
		{
			if (m_bRudderLeft)
			{
				pRudder->setAileronAngle_norm(-1.0);
			}
			else
			if (m_bRudderRight)
			{
				pRudder->setAileronAngle_norm(1.0);
			}
			else
			{
				pRudder->setAileronAngle_norm(0.0);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::keyPressEvent(QKeyEvent* event)
{
	CStandardController::keyPressEvent(event);

	CWing* pLeftWing = m_rLeftWingTarget.component();
	CWing* pRightWing = m_rRightWingTarget.component();
	CEngine* pEngine1 = m_rEngine1Target.component();
	CEngine* pEngine2 = m_rEngine2Target.component();

	switch (event->key())
	{
		case Qt::Key_D:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronRight, CQ3DEvent::Press));
			break;
		case Qt::Key_Q:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronLeft, CQ3DEvent::Press));
			break;
		case Qt::Key_S:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseUp, CQ3DEvent::Press));
			break;
		case Qt::Key_Z:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseDown, CQ3DEvent::Press));
			break;
		case Qt::Key_C:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderRight, CQ3DEvent::Press));
			break;
		case Qt::Key_W:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderLeft, CQ3DEvent::Press));
			break;
		case Qt::Key_8:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFront, CQ3DEvent::Press));
			break;
		case Qt::Key_9:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontRight, CQ3DEvent::Press));
			break;
		case Qt::Key_6:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookRight, CQ3DEvent::Press));
			break;
		case Qt::Key_3:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBackRight, CQ3DEvent::Press));
			break;
		case Qt::Key_2:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBack, CQ3DEvent::Press));
			break;
		case Qt::Key_1:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBackLeft, CQ3DEvent::Press));
			break;
		case Qt::Key_4:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookLeft, CQ3DEvent::Press));
			break;
		case Qt::Key_7:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontLeft, CQ3DEvent::Press));
			break;
		case Qt::Key_5:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontDown, CQ3DEvent::Press));
			break;
		case Qt::Key_PageUp:
			{
				if (pEngine1 != NULL)
				{
					pEngine1->setCurrentFuelFlow_norm(pEngine1->getCurrentFuelFlow_norm() + 0.1);
				}

				if (pEngine2 != NULL)
				{
					pEngine2->setCurrentFuelFlow_norm(pEngine2->getCurrentFuelFlow_norm() + 0.1);
				}
			}
			break;
		case Qt::Key_PageDown:
			{
				if (pEngine1 != NULL)
				{
					pEngine1->setCurrentFuelFlow_norm(pEngine1->getCurrentFuelFlow_norm() - 0.1);
				}

				if (pEngine2 != NULL)
				{
					pEngine2->setCurrentFuelFlow_norm(pEngine2->getCurrentFuelFlow_norm() - 0.1);
				}
			}
			break;
		case Qt::Key_M:
			{
				if (pLeftWing != NULL && pRightWing != NULL)
				{
					pLeftWing->setFlapsPosition_norm(pLeftWing->getFlapsPosition_norm() + 0.2);
					pRightWing->setFlapsPosition_norm(pRightWing->getFlapsPosition_norm() + 0.2);
				}
			}
			break;
		case Qt::Key_P:
			{
				if (pLeftWing != NULL && pRightWing != NULL)
				{
					pLeftWing->setFlapsPosition_norm(pLeftWing->getFlapsPosition_norm() - 0.2);
					pRightWing->setFlapsPosition_norm(pRightWing->getFlapsPosition_norm() - 0.2);
				}
			}
			break;
	}
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::keyReleaseEvent(QKeyEvent *event)
{
	CStandardController::keyReleaseEvent(event);

	switch (event->key())
	{
		case Qt::Key_D:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronRight, CQ3DEvent::Release));
			break;
		case Qt::Key_Q:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronLeft, CQ3DEvent::Release));
			break;
		case Qt::Key_S:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseUp, CQ3DEvent::Release));
			break;
		case Qt::Key_Z:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseDown, CQ3DEvent::Release));
			break;
		case Qt::Key_C:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderRight, CQ3DEvent::Release));
			break;
		case Qt::Key_W:
			generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderLeft, CQ3DEvent::Release));
			break;
	}
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::q3dEvent(CQ3DEvent* event)
{
	CStandardController::q3dEvent(event);

	CComponent* pRotationTarget = m_rRotationTarget.component();

	if (event->getName() == Q3DEvent_AileronRight)
	{
		m_bAileronRight = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_AileronLeft)
	{
		m_bAileronLeft = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_NoseUp)
	{
		m_bNoseUp = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_NoseDown)
	{
		m_bNoseDown = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_RudderRight)
	{
		m_bRudderRight = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_RudderLeft)
	{
		m_bRudderLeft = (event->getAction() == CQ3DEvent::Press);
	}
	else if (event->getName() == Q3DEvent_LookFront)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3((Math::Pi / 20.0) * 1.0, (Math::Pi / 4.0) * 0.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookFrontRight)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 1.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookRight)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 2.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookBackRight)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 3.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookBack)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 4.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookBackLeft)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 5.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookLeft)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 6.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookFrontLeft)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3(0.0, (Math::Pi / 4.0) * 7.0, 0.0));
			}
		}
	}
	else if (event->getName() == Q3DEvent_LookFrontDown)
	{
		if (event->getAction() == CQ3DEvent::Press)
		{
			if (pRotationTarget != NULL)
			{
				pRotationTarget->setOriginRotation(CVector3((Math::Pi / 4.0) * 1.0, (Math::Pi / 4.0) * 1.0, 0.0));
			}
		}
	}
}
