
// Std
#include <cmath>

// COTS
#ifdef WIN32
#include <SFML/Window/Joystick.hpp>
#endif

// Application
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CJoystick. \br\br
    \a uiJoystickIndex is the joystick index.
*/
CJoystick::CJoystick(unsigned int uiJoystickIndex)
: m_bConnected(false)
, m_uiJoystickIndex(uiJoystickIndex)
, m_uiNumButtons(2)
, m_uiNumAxis(4)
#ifndef WIN32
, m_pReader(new CJoystickReader(uiJoystickIndex))
#endif
{
#ifndef WIN32
    m_bConnected = m_pReader->connected();
    m_mButtonStates = m_pReader->buttonStates();
    m_dAxisStates = m_pReader->axisStates();
#endif
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CJoystick.
*/
CJoystick::~CJoystick()
{
#ifndef WIN32
    delete m_pReader;
#endif
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this object using \a dDeltaTime, which is the elapsed seconds since the last frame.
*/
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

#else

    m_pReader->update();

    m_bConnected = m_pReader->connected();
    m_mButtonStates = m_pReader->buttonStates();
    m_dAxisStates = m_pReader->axisStates();

#endif
}
