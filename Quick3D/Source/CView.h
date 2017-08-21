
#pragma once

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

// qt-plus
#include "CAverager.h"

// Application
#include "quick3d_global.h"
#include "CGLWidgetScene.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

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
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CView(QWidget *parent = 0);

    //! Constructor using a scene
    CView(CGLWidgetScene* pScene, QWidget *parent = 0);

    //! Destructor
    virtual ~CView();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setScene(CGLWidgetScene* pScene);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the scene
    CGLWidgetScene* scene() { return m_pScene; }

    //! Returns the constant scene
    const CGLWidgetScene* scene() const { return m_pScene; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void update(double dDeltaTime);

    //!
    void testPerlin();

    //-------------------------------------------------------------------------------------------------
    // Protected methods
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
    // Signals
    //-------------------------------------------------------------------------------------------------

signals:

    void keyPress(QKeyEvent* event);
    void keyRelease(QKeyEvent* event);
    void mousePress(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
    void wheel(QWheelEvent *event);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CGLWidgetScene*	m_pScene;
};
