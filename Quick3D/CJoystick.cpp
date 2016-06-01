
// COTS
#include <SFML/Window/Joystick.hpp>

// Application
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------

CJoystick::CJoystick(int iJoystickIndex)
: m_bConnected(false)
, m_iJoystickIndex(iJoystickIndex)
, m_iNumButtons(2)
, m_iNumAxis(4)
{
}

//-------------------------------------------------------------------------------------------------

CJoystick::~CJoystick()
{
}

//-------------------------------------------------------------------------------------------------

void CJoystick::update(double dDeltaTime)
{
	m_bConnected = sf::Joystick::IsConnected(m_iJoystickIndex);

	for (int iButtonIndex = 0; iButtonIndex < m_iNumButtons; iButtonIndex++)
	{
		m_mButtonStates[iButtonIndex] = sf::Joystick::IsButtonPressed(m_iJoystickIndex, iButtonIndex);
	}

	for (int iAxisIndex = 0; iAxisIndex < m_iNumAxis; iAxisIndex++)
	{
		sf::Joystick::Axis theAxis = (sf::Joystick::Axis) iAxisIndex;

		m_dAxisStates[iAxisIndex] = (sf::Joystick::GetAxisPosition(m_iJoystickIndex, theAxis) / 100.0f);
	}
}
