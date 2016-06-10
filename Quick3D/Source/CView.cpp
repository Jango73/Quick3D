
// Qt
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

// Application
#include "CLogManager.h"
#include "CView.h"
#include "CController.h"
#include "CPerlin.h"
#include "CRain.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CView::CView(QWidget *parent)
: QGraphicsView(parent)
, m_pScene(NULL)
{
	LOG_DEBUG("CView::CView()");

	// setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setRenderHint(QPainter::Antialiasing, false);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//-------------------------------------------------------------------------------------------------

CView::CView(CGLWidgetScene* pScene, QWidget *parent)
    : QGraphicsView(parent)
    , m_pScene(NULL)
{
    LOG_DEBUG("CView::CView()");

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
	LOG_DEBUG("CView::~CView()");
}

//-------------------------------------------------------------------------------------------------

void CView::setScene(CGLWidgetScene* pScene)
{
	//-----------------------------------------------
	// Initialisation OpenGL

	m_pScene = pScene;

	if (m_pScene != NULL)
	{
		m_pScene->makeCurrentRenderingContext();

		setViewport(m_pScene);
	}
}

//-------------------------------------------------------------------------------------------------

void CView::keyPressEvent(QKeyEvent* event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit keyPress(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->keyPressEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::keyReleaseEvent(QKeyEvent *event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit keyRelease(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->keyReleaseEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::mousePressEvent(QMouseEvent* event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit mousePress(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->mousePressEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit mouseRelease(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->mouseReleaseEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit mouseMove(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->mouseMoveEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::wheelEvent(QWheelEvent *event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			emit wheel(event);
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				m_pScene->getController()->wheelEvent(event);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::focusOutEvent(QFocusEvent* event)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
		}
		else
		{
			if (m_pScene->getController() != NULL)
			{
				{
					QKeyEvent tEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
					m_pScene->getController()->keyReleaseEvent(&tEvent);
				}

				{
					QKeyEvent tEvent(QEvent::KeyRelease, Qt::Key_Alt, Qt::NoModifier);
					m_pScene->getController()->keyReleaseEvent(&tEvent);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CView::update(double dDeltaTimeS)
{
	if (m_pScene != NULL)
	{
		if (m_pScene->getEditMode())
		{
			dDeltaTimeS = 0.0;
		}

		double dTime = m_pScene->getTime() + dDeltaTimeS;

		m_pScene->setTime(dTime);
		m_pScene->setGeometry(this->frameRect());
		m_pScene->updateGL();

		foreach (int iIndex, m_pScene->getViewports().keys())
		{
			m_pScene->getViewports()[iIndex]->update(dDeltaTimeS);
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
			double dValue = perlin->getNoise(CVector3(x, 0.0, z) * 0.001);

			if (dValue < dMin) dMin = dValue;
			if (dValue > dMax) dMax = dValue;
		}
	}

	LOG_DEBUG(QString("Min and max noise : %1, %2").arg(dMin).arg(dMax));
}
