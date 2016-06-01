
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------

class IController
{

public:

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	virtual void keyPressEvent(QKeyEvent* event) = 0;

	//!
	virtual void keyReleaseEvent(QKeyEvent *event) = 0;

	//!
	virtual void mousePressEvent(QMouseEvent* event) = 0;

	//!
	virtual void mouseReleaseEvent(QMouseEvent* event) = 0;

	//!
	virtual void mouseMoveEvent(QMouseEvent* event) = 0;

	//!
	virtual void wheelEvent(QWheelEvent *event) = 0;

	//!
	virtual void joystickEvent(CJoystick* pJoystick) = 0;
};
