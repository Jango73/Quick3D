
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
    // Initialisation OpenGL

    m_pScene = pScene;

    if (m_pScene != nullptr)
    {
        m_pScene->makeCurrentRenderingContext();

        setViewport(m_pScene);
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
                m_pScene->controller()->keyPressEvent(event);
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
        m_pScene->updateGL();

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
