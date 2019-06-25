
// Std
#include <cmath>

// COTS
#ifdef WIN32
#include <SFML/Window/Joystick.hpp>
#endif

// Application
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------

CJoystick::CJoystick(unsigned int uiJoystickIndex)
: m_bConnected(false)
, m_uiJoystickIndex(uiJoystickIndex)
, m_uiNumButtons(2)
, m_uiNumAxis(4)
{
}

//-------------------------------------------------------------------------------------------------

CJoystick::~CJoystick()
{
}

//-------------------------------------------------------------------------------------------------

void CJoystick::update(double dDeltaTime)
{
    Q_UNUSED(dDeltaTime)

#ifdef WIN32

    m_bConnected = sf::Joystick::IsConnected(m_uiJoystickIndex);

    for (unsigned int uiButtonIndex = 0; uiButtonIndex < m_uiNumButtons; uiButtonIndex++)
	{
        m_mButtonStates[uiButtonIndex] = sf::Joystick::IsButtonPressed(m_uiJoystickIndex, uiButtonIndex);
	}

    for (unsigned int uiAxisIndex = 0; uiAxisIndex < m_uiNumAxis; uiAxisIndex++)
	{
        sf::Joystick::Axis theAxis = sf::Joystick::Axis(uiAxisIndex);

        m_dAxisStates[uiAxisIndex] = double(sf::Joystick::GetAxisPosition(m_uiJoystickIndex, theAxis) / 100.0f);

        if (std::fabs(m_dAxisStates[uiAxisIndex]) < 0.1)
        {
            m_dAxisStates[uiAxisIndex] = 0.0;
        }
	}

#endif
}
