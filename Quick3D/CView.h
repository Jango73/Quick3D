
#ifndef CVIEW_H
#define CVIEW_H

#include "quick3d_global.h"

// Qt
#include <QObject>
#include <QPaintEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGLWidget>
#include <QTcpSocket>
#include <QTcpServer>

#define _WINSOCKAPI_

// Application
#include "CAverager.h"
#include "CGLWidgetScene.h"

//-------------------------------------------------------------------------------------------------

class CView;
class CSkyBox;
class CWater;
class CRain;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CView : public QGraphicsView
{
	Q_OBJECT

	friend class CViewServer;

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CView(QWidget *parent = 0);

	//! Destructeur
	virtual ~CView();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setScene(CGLWidgetScene* pScene);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Getter sur la scene
	CGLWidgetScene* scene() { return m_pScene; }

	//! Getter sur la scene
	const CGLWidgetScene* scene() const { return m_pScene; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void update(double dDeltaTime);

	//!
	void testPerlin();

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void focusOutEvent(QFocusEvent* event);

	//-------------------------------------------------------------------------------------------------
	// Signaux
	//-------------------------------------------------------------------------------------------------

signals:

	void keyPress(QKeyEvent* event);
	void keyRelease(QKeyEvent* event);
	void mousePress(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void wheel(QWheelEvent *event);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	CGLWidgetScene*	m_pScene;
};

#endif // CVIEW_H
