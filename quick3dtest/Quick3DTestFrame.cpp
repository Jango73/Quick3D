
// Qt
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QRandomGenerator>
#include <QKeyEvent>

// Quick3D
#include "CEnvironmentDefaults.h"

// qt-plus
#include "CTimeSampler.h"

// Quick3D
#include "CConsoleBoard.h"
#include "CQ3DConstants.h"
#include "CComponentFactory.h"
#include "CComponentLoader.h"
#include "CWorkerManager.h"
#include "CTerrain.h"
#include "CWorldChunk.h"
#include "CVehicle.h"
#include "CWheel.h"
#include "CWorldTerrain.h"
#include "CController.h"
#include "CPhysicsWorld.h"
#include "CRemoteControlServer.h"
#include "CUiRemoteControlHandler.h"

// Application
#include "Quick3DTest.h"

#ifdef _MSC_VER
#include "GeneratedFiles/ui_ExportDialog.h"
#else
#include "ui_ExportDialog.h"
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------

void Quick3DTest::onTimer()
{
    m_tTimer.stop();

    if (m_pScene != nullptr && m_bRun == true)
    {
        QDateTime tCurrentTime = QDateTime::currentDateTime();
        double dDeltaTime = double(m_tPreviousTime.msecsTo(tCurrentTime)) / 1000.0;

        // Stepped frames advance a fixed quantum: wall-clock time kept flowing while paused
        if (m_iSingleSteps > 0)
        {
            dDeltaTime = MinimumFrameDurationS;
        }

        if (dDeltaTime < MinimumFrameDurationS)
        {
            int iRemainingMs = int((MinimumFrameDurationS - dDeltaTime) * 1000.0);

            if (iRemainingMs < 1)
            {
                iRemainingMs = 1;
            }

            m_tTimer.start(iRemainingMs);
            return;
        }

        m_tPreviousTime = tCurrentTime;

        m_iFrames++;

        m_pScene->updateScene(dDeltaTime);
        m_pView->update(dDeltaTime);

        {
            CScopedTimeSample tUiSample(QString("Frame_SetText"));
            ui.m_lInfo->setText(m_pScene->debugInfo());
        }

        if (m_bCheckRender == true)
        {
            m_iCheckRenderFrames++;

            if (m_iCheckRenderFrames >= 600 && qEnvironmentVariableIsSet("QUICK3D_ENDLESS") == false)
            {
                QImage tFrameBuffer = m_pScene->grabFramebuffer();
                tFrameBuffer.save(QCoreApplication::applicationDirPath() + "/../../tmp/check-render-full.png");
                QImage tCaptured = tFrameBuffer.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::FastTransformation);

                int iBlackPixels = 0;
                long lRedTotal = 0;
                long lGreenTotal = 0;
                long lBlueTotal = 0;
                QSet<QRgb> tColors;

                for (int y = 0; y < tCaptured.height(); y++)
                {
                    for (int x = 0; x < tCaptured.width(); x++)
                    {
                        QRgb tPixel = tCaptured.pixel(x, y);
                        QColor tColor(tPixel);

                        if (tColor.red() < 16 && tColor.green() < 16 && tColor.blue() < 16)
                        {
                            iBlackPixels++;
                        }

                        lRedTotal += tColor.red();
                        lGreenTotal += tColor.green();
                        lBlueTotal += tColor.blue();
                        tColors.insert(tPixel);
                    }
                }

                int iPixelCount = tCaptured.width() * tCaptured.height();
                double dBlackRatio = iPixelCount > 0 ? double(iBlackPixels) / double(iPixelCount) : -1.0;

                LOG_DEBUG(QString("Render check : size = %1x%2, black pixel ratio = %3, average = (%4, %5, %6), distinct colors = %7")
                    .arg(tCaptured.width()).arg(tCaptured.height()).arg(dBlackRatio)
                    .arg(lRedTotal / qMax(1, iPixelCount)).arg(lGreenTotal / qMax(1, iPixelCount)).arg(lBlueTotal / qMax(1, iPixelCount))
                    .arg(tColors.count()));

                LOG_DEBUG(QString("Scene : %1").arg(m_pScene->debugInfo().replace("\n", " | ")));

                LOG_DEBUG(QString("Stats : frustum tests = %1").arg(m_pScene->m_tStatistics.m_iNumFrustumTests));

                {
                    QVector<QSP<CLight> > vLights = m_pScene->lights();
                    QString sSunDiffuse = "none";

                    foreach (QSP<CLight> pLight, vLights)
                    {
                        if (pLight != nullptr && pLight->tag() == "SUN" && pLight->material() != nullptr)
                        {
                            sSunDiffuse = QString("(%1, %2, %3)").arg(pLight->material()->diffuse().X).arg(pLight->material()->diffuse().Y).arg(pLight->material()->diffuse().Z);
                        }
                    }

                    LOG_DEBUG(QString("Lights : count = %1, sun diffuse = %2, fog = (%3, %4, %5), quality = %6")
                        .arg(vLights.count()).arg(sSunDiffuse)
                        .arg(m_pScene->fog().color().X).arg(m_pScene->fog().color().Y).arg(m_pScene->fog().color().Z)
                        .arg(m_pScene->shaderQuality()));
                }

                LOG_DEBUG(QString("Native : scene native flag = %1, handle = %2")
                    .arg(m_pScene->testAttribute(Qt::WA_NativeWindow))
                    .arg((quintptr) m_pScene->windowHandle()));

                LOG_DEBUG(QString("Native2 : scene winId = %1, view winId = %2")
                    .arg(m_pScene->winId()).arg(m_pView->winId()));

                LOG_DEBUG(QString("Handles : scene handle = %1 valid = %2, view handle = %3 valid = %4")
                    .arg((quintptr) m_pScene->windowHandle()).arg(m_pScene->windowHandle() != nullptr && m_pScene->windowHandle()->isExposed())
                    .arg((quintptr) m_pView->windowHandle()).arg(m_pView->windowHandle() != nullptr && m_pView->windowHandle()->isExposed()));

                LOG_DEBUG(QString("Parents : scene parent = %1 (view = %2), view parent = %3, main handle = %4 exposed = %5")
                    .arg((quintptr) m_pScene->parent()).arg((quintptr) m_pView)
                    .arg((quintptr) m_pView->parent())
                    .arg((quintptr) windowHandle()).arg(windowHandle() != nullptr && windowHandle()->isExposed()));

                LOG_DEBUG(QString("Widgets : Render1 = %1, view size = %2x%3 visible = %4, Render1 size = %5x%6 visible = %7")
                    .arg((quintptr) ui.Render1)
                    .arg(m_pView->width()).arg(m_pView->height()).arg(m_pView->isVisibleTo(this))
                    .arg(ui.Render1 != nullptr ? ui.Render1->width() : -1).arg(ui.Render1 != nullptr ? ui.Render1->height() : -1).arg(ui.Render1 != nullptr && ui.Render1->isVisibleTo(this)));

                LOG_DEBUG(QString("Windows : scene isWindow = %1, scene window = %2 (main = %3), view isWindow = %4")
                    .arg(m_pScene->isWindow()).arg((quintptr) m_pScene->window()).arg((quintptr) this)
                    .arg(m_pView->isWindow()));

                LOG_DEBUG(QString("ViewProbe : graphics scene = %1, viewport is GL widget = %2, viewport class = %3")
                    .arg((quintptr) m_pView->QGraphicsView::scene())
                    .arg(m_pView->viewport() == (QWidget*) m_pScene)
                    .arg(m_pView->viewport() != nullptr ? m_pView->viewport()->metaObject()->className() : "null"));

                LOG_DEBUG(QString("Geometry : scene = (%1, %2, %3x%4) visible = %5, view = (%6, %7, %8x%9)")
                    .arg(m_pScene->x()).arg(m_pScene->y()).arg(m_pScene->width()).arg(m_pScene->height()).arg(m_pScene->isVisible())
                    .arg(m_pView->x()).arg(m_pView->y()).arg(m_pView->width()).arg(m_pView->height()));

                LOG_DEBUG(QString("SceneState : visible = %1, visibleToMain = %2, exposed = %3, updatesEnabled = %4, parent = %5")
                    .arg(m_pScene->isVisible()).arg(m_pScene->isVisibleTo(this))
                    .arg(m_pScene->windowHandle() != nullptr && m_pScene->windowHandle()->isExposed())
                    .arg(m_pScene->updatesEnabled()).arg((quintptr) m_pScene->parentWidget()));

                QScreen* pScreen = QApplication::primaryScreen();

                if (pScreen != nullptr)
                {
                    QPoint tViewPosition = m_pView->mapToGlobal(QPoint(0, 0));
                    QPixmap tScreenShot = pScreen->grabWindow(0, tViewPosition.x(), tViewPosition.y(), m_pView->width(), m_pView->height());
                    QImage tSmall = tScreenShot.toImage().scaled(16, 16, Qt::IgnoreAspectRatio, Qt::FastTransformation);

                    int iScreenBlack = 0;
                    long lScreenRed = 0;
                    long lScreenGreen = 0;
                    long lScreenBlue = 0;

                    for (int y = 0; y < tSmall.height(); y++)
                    {
                        for (int x = 0; x < tSmall.width(); x++)
                        {
                            QColor tColor(tSmall.pixel(x, y));

                            if (tColor.red() < 16 && tColor.green() < 16 && tColor.blue() < 16)
                            {
                                iScreenBlack++;
                            }

                            lScreenRed += tColor.red();
                            lScreenGreen += tColor.green();
                            lScreenBlue += tColor.blue();
                        }
                    }

                    int iScreenPixels = tSmall.width() * tSmall.height();

                    LOG_DEBUG(QString("Screen check : black pixel ratio = %1, average = (%2, %3, %4)")
                        .arg(iScreenPixels > 0 ? double(iScreenBlack) / double(iScreenPixels) : -1.0)
                        .arg(lScreenRed / qMax(1, iScreenPixels)).arg(lScreenGreen / qMax(1, iScreenPixels)).arg(lScreenBlue / qMax(1, iScreenPixels)));
                }

                QApplication::quit();
            }
        }
    }

    if (m_iSingleSteps > 0)
    {
        m_iSingleSteps--;

        if (m_iSingleSteps == 0)
        {
            m_bRun = false;
            ui.m_bPause->setChecked(true);
        }
    }

    m_tTimer.start(FrameTimerPollIntervalMs);
}

//-------------------------------------------------------------------------------

/*!
    Pauses or resumes the frame loop through the Pause toggle button.
*/
void Quick3DTest::onPauseToggled(bool bChecked)
{
    m_iSingleSteps = 0;
    m_bRun = bChecked == false;
    ui.m_bStep->setEnabled(bChecked);
}

//-------------------------------------------------------------------------------

/*!
    Advances a single frame while paused, stays paused afterwards.
*/
void Quick3DTest::onStepClicked()
{
    if (m_bRun == false && m_iSingleSteps == 0)
    {
        m_iSingleSteps = 1;
        m_bRun = true;
    }
}
