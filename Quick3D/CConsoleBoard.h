
#ifndef CCONSOLEBOARD_H
#define CCONSOLEBOARD_H

#include "quick3d_global.h"

#ifdef linux
#include <stdio.h>
#endif

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

#include "CSingleton.h"

//-------------------------------------------------------------------------------------------------

#define LOG_VALUE(a,b)	CConsoleBoard::getInstance()->setNameValue(a, b)

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CConsoleBoard : public QObject, public CSingleton<CConsoleBoard>
{
	Q_OBJECT

	friend class CSingleton<CConsoleBoard>;

protected:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	CConsoleBoard();

	//! Destructeur
	virtual ~CConsoleBoard();

public:

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
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
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//-------------------------------------------------------------------------------------------------
	// Slots
	//-------------------------------------------------------------------------------------------------

protected slots:

	void onTimeout();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	bool					m_bActive;
	QTimer					m_tTimer;
	QMutex					m_tMutex;
	QMap<QString, QString>	m_mValues;
};

#endif // CCONSOLEBOARD_H
