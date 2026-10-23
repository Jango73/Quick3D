
// Qt
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

// qt-plus
#include "CLogger.h"

// Application
#include "CView.h"
#include "CController.h"
#include "CPerlin.h"
#include "CRain.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CView::CView(QWidget *parent)
    : QGraphicsView(parent)
    , m_pScene(nullptr)
{
    // setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//-------------------------------------------------------------------------------------------------

CView::CView(CGLWidgetScene* pScene, QWidget *parent)
    : QGraphicsView(parent)
    , m_pScene(nullptr)
{
    // setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(pScene);
}

//-------------------------------------------------------------------------------------------------

CView::~CView()
{
}

//-------------------------------------------------------------------------------------------------

void CView::setScene(CGLWidgetScene* pScene)
{
    //-----------------------------------------------
    // OpenGL initialization

    m_pScene = pScene;

    if (m_pScene != nullptr)
    {
        // The scene is hosted as a plain child widget, never as the graphics
        // view viewport : QGraphicsView intercepts viewport paint events in
        // viewportEvent, so QOpenGLWidget::paintGL never runs while set as viewport.
        // No WA_NativeWindow here : QOpenGLWidget renders to an offscreen buffer
        // composed by Qt, a forced native window breaks onscreen presentation.
        m_pScene->setParent(this);
        m_pScene->setGeometry(frameRect());
        m_pScene->show();
        m_pScene->raise();

        setFocusPolicy(Qt::StrongFocus);
        m_pScene->setFocusPolicy(Qt::StrongFocus);
        m_pScene->setFocusProxy(this);
        m_pScene->installEventFilter(this);

        m_pScene->makeCurrentRenderingContext();
    }
}

//-------------------------------------------------------------------------------------------------

void CView::keyPressEvent(QKeyEvent* event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit keyPress(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                LOG_DEBUG(QString("Key press %1 scan %2 to %3").arg(event->key()).arg(event->nativeScanCode()).arg(m_pScene->controller()->qualifiedName()));
                m_pScene->controller()->keyPressEvent(event);
            }
            else
            {
                LOG_DEBUG(QString("Key press %1 scan %2 without controller").arg(event->key()).arg(event->nativeScanCode()));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit keyRelease(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                m_pScene->controller()->keyReleaseEvent(event);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::mousePressEvent(QMouseEvent* event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit mousePress(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                m_pScene->controller()->mousePressEvent(event);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit mouseRelease(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                m_pScene->controller()->mouseReleaseEvent(event);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit mouseMove(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                m_pScene->controller()->mouseMoveEvent(event);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::wheelEvent(QWheelEvent *event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            emit wheel(event);
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                m_pScene->controller()->wheelEvent(event);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::focusOutEvent(QFocusEvent* event)
{
    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
        }
        else
        {
            if (m_pScene->controller() != nullptr)
            {
                {
                    QKeyEvent tEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
                    m_pScene->controller()->keyReleaseEvent(&tEvent);
                }

                {
                    QKeyEvent tEvent(QEvent::KeyRelease, Qt::Key_Alt, Qt::NoModifier);
                    m_pScene->controller()->keyReleaseEvent(&tEvent);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool CView::eventFilter(QObject* pObject, QEvent* pEvent)
{
    if (pObject == m_pScene && m_pScene != nullptr && pEvent != nullptr)
    {
        switch (pEvent->type())
        {
        case QEvent::MouseButtonPress:
            mousePressEvent(static_cast<QMouseEvent*>(pEvent));
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(static_cast<QMouseEvent*>(pEvent));
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(static_cast<QMouseEvent*>(pEvent));
            break;
        case QEvent::Wheel:
            wheelEvent(static_cast<QWheelEvent*>(pEvent));
            break;
        case QEvent::FocusOut:
            focusOutEvent(static_cast<QFocusEvent*>(pEvent));
            break;
        default:
            break;
        }
    }

    return QGraphicsView::eventFilter(pObject, pEvent);
}

//-------------------------------------------------------------------------------------------------

void CView::update(double dDeltaTimeS)
{
    if (dDeltaTimeS < 0.0) dDeltaTimeS = 0.0;
    if (dDeltaTimeS > 1.0) dDeltaTimeS = 1.0;

    if (m_pScene != nullptr)
    {
        if (m_pScene->editMode())
        {
            dDeltaTimeS = 0.0;
        }

        double dTime = m_pScene->time() + dDeltaTimeS;

        m_pScene->setTime(dTime);
        m_pScene->setGeometry(this->frameRect());
        m_pScene->update();

        foreach (int iIndex, m_pScene->viewports().keys())
        {
            m_pScene->viewports()[iIndex]->update(dDeltaTimeS);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CView::testPerlin()
{
    // Perlin min and max test
    CPerlin* perlin = CPerlin::getInstance();

    double dMin = 99.0;
    double dMax = -99.0;

    for (double z = 0.0; z < 10000.0; z += 25.0)
    {
        for (double x = 0.0; x < 10000.0; x += 25.0)
        {
            double dValue = perlin->noise(CVector3(x, 0.0, z) * 0.001);

            if (dValue < dMin) dMin = dValue;
            if (dValue > dMax) dMax = dValue;
        }
    }

    LOG_METHOD_DEBUG(QString("Min and max noise : %1, %2").arg(dMin).arg(dMax));
}
