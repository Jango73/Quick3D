
#ifndef VIRTUALPILOT_H
#define VIRTUALPILOT_H

// QT
#include <QtGlobal>

#if QT_VERSION >= 0x050000
    #include <QMainWindow>
#else
    #include <QtGui/QMainWindow>
#endif

// Application
#include "ui_VirtualPilot.h"

#include "CGLWidgetScene.h"
#include "CComponent.h"
#include "CView.h"
#include "CAutoTerrain.h"

class VirtualPilot : public QMainWindow
{
	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
#if QT_VERSION >= 0x050000
    VirtualPilot(QString sSceneFileName, QWidget *parent = 0);
#else
    VirtualPilot(QString sSceneFileName, QWidget *parent = 0, Qt::WFlags flags = 0);
#endif

	//! Destructeur
	virtual ~VirtualPilot();

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void readPreferences();

	//!
	void loadScene(QString sFileName);

	//!
	void loadVehicle(QString sFileName);

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//!
	virtual void resizeEvent(QResizeEvent *event);

	//!
	int randInt(int low, int high);

	//-------------------------------------------------------------------------------------------------
	// Slots
	//-------------------------------------------------------------------------------------------------

private slots:

	void onTimer();
	void onLoadSceneClicked();
	void onLoadVehicleClicked();
	void onPreferencesClicked();
	void onQuitClicked();
	void onDumpSceneClicked();
	void onResize();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Ui::VirtualPilotClass			ui;
	CGLWidgetScene*					m_pScene;
	CView*							m_pView;
	QTimer							m_tTimer;
	CAverager<double>				m_FPS;
	QDateTime						m_tPreviousTime;
	QString							m_sPathVehicles;
	bool							m_bProcessEvents;
	bool							m_bRun;
};

#endif // VIRTUALPILOT_H
