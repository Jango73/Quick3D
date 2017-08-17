
#pragma once

// Std
#ifdef linux
#include <stdio.h>
#endif

// Qt
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#define LOG_VALUE(a,b)	CConsoleBoard::getInstance()->setNameValue(a, b)

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CConsoleBoard : public QObject, public CSingleton<CConsoleBoard>
{
	Q_OBJECT

	friend class CSingleton<CConsoleBoard>;

protected:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	CConsoleBoard();

	//! Destructor
	virtual ~CConsoleBoard();

public:

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Control methods
	//-------------------------------------------------------------------------------------------------

	//!
	void start();

	//!
	void stop();

	//!
	void refresh();

	//!
	void clearScreen();

	//!
	void printAt(int iPosX, int iPosY, QString sText);

	//!
	void setNameValue(const QString& sName, const QString& sValue);

	//-------------------------------------------------------------------------------------------------
	// Protected methods
	//-------------------------------------------------------------------------------------------------

protected:

	//-------------------------------------------------------------------------------------------------
	// Slots
	//-------------------------------------------------------------------------------------------------

protected slots:

	void onTimeout();

	//-------------------------------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------------------------------

protected:

	bool					m_bActive;
	QTimer					m_tTimer;
	QMutex					m_tMutex;
	QMap<QString, QString>	m_mValues;
};
