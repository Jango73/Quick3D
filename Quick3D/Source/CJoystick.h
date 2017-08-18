
#pragma once

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CJoystick
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//!
	CJoystick(int iJoystickIndex);

	//!
	~CJoystick();

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	bool connected() const { return m_bConnected; }

	//
    QMap<int, bool>& buttonStates() { return m_mButtonStates; }

	//
    QMap<int, double>& axisStates() { return m_dAxisStates; }

	//-------------------------------------------------------------------------------------------------
	// Control methods
	//-------------------------------------------------------------------------------------------------

	//!
	void update(double dDeltaTime);

protected:

	bool				m_bConnected;
	int					m_iJoystickIndex;
	int					m_iNumButtons;
	int					m_iNumAxis;
	QMap<int, bool>		m_mButtonStates;
	QMap<int, double>	m_dAxisStates;
};
