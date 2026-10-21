
// Qt
#include <QFileDialog>
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

//-------------------------------------------------------------------------------------------------

static const int FrameTimerPollIntervalMs = 5;
static const double MaximumFrameRateFps = 60.0;
static const double MinimumFrameDurationS = 1.0 / MaximumFrameRateFps;

//-------------------------------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
Quick3DTest::Quick3DTest(QString sSceneFileName, QWidget *parent)
    : QMainWindow(parent)
    #else
Quick3DTest::Quick3DTest(QString sSceneFileName, QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    #endif
    , m_tTimer(this)
    , m_bProcessEvents(true)
    , m_bRun(true)
    , m_bCheckRender(false)
    , m_iCheckRenderFrames(0)
    , m_pRemoteHandler(nullptr)
    , m_pRemoteServer(nullptr)
    , m_pFogColorDialog(nullptr)
{
    // CConsoleBoard::getInstance()->start();

    ui.setupUi(this);

    CComponentFactory::getInstance()->registerCoreComponents();

    m_pScene = new CGLWidgetScene();
    m_pView = new CView(m_pScene, ui.Render1);

    if (qEnvironmentVariableIsSet("Q3D_REMOTE_PORT") == true)
    {
        bool bPortOk = false;
        int iPort = qEnvironmentVariableIntValue("Q3D_REMOTE_PORT", &bPortOk);

        if (bPortOk == false)
        {
            iPort = 8181;
        }

        m_pRemoteHandler = new CUiRemoteControlHandler(this, this);
        m_pRemoteServer = new CRemoteControlServer(m_pRemoteHandler, quint16(iPort), this);
    }

    // Event handlers
    connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

    connect(ui.actionLoad_scene, SIGNAL(triggered()), this, SLOT(onLoadSceneClicked()));
    connect(ui.actionClear_scene, SIGNAL(triggered()), this, SLOT(onClearSceneClicked()));
    connect(ui.actionExport_terrain, SIGNAL(triggered()), this, SLOT(onExportTerrainClicked()));
    connect(ui.actionGenerate_matrix, SIGNAL(triggered()), this, SLOT(onGenerateMatrixClicked()));
    connect(ui.actionDump_scene, SIGNAL(triggered()), this, SLOT(onDumpSceneClicked()));

    connect(ui.m_sTime, SIGNAL(valueChanged(int)), this, SLOT(onTimeChanged(int)));
    connect(ui.m_sFogLevel, SIGNAL(valueChanged(int)), this, SLOT(onFogLevelChanged(int)));
    connect(ui.m_bEnvironment, SIGNAL(clicked()), this, SLOT(onEnvironmentClicked()));

    ui.m_sFogLevel->setGeometry(10, 20, 68, 20);

    QPushButton* pFogColorButton = new QPushButton("Color", ui.groupBox_2);
    pFogColorButton->setGeometry(82, 18, 46, 22);
    pFogColorButton->setFocusPolicy(Qt::NoFocus);
    connect(pFogColorButton, SIGNAL(clicked()), this, SLOT(onFogColorClicked()));
    connect(ui.m_sWindLevel, SIGNAL(valueChanged(int)), this, SLOT(onWindLevelChanged(int)));
    connect(ui.m_sShaderQuality, SIGNAL(valueChanged(int)), this, SLOT(onShaderQualityChanged(int)));
    connect(ui.m_sTerrainRes, SIGNAL(valueChanged(int)), this, SLOT(onTerrainResChanged(int)));
    connect(ui.m_sMoveSpeed, SIGNAL(valueChanged(int)), this, SLOT(onMoveSpeedChanged(int)));
    connect(ui.m_sOverlookFOV, SIGNAL(valueChanged(int)), this, SLOT(onOverlookFOVChanged(int)));

    connect(ui.m_chkRain, SIGNAL(clicked()), this, SLOT(onRainClicked()));
    connect(ui.m_chkWideFOV, SIGNAL(clicked()), this, SLOT(onWideFOVClicked()));
    connect(ui.m_chkNarrowFOV, SIGNAL(clicked()), this, SLOT(onNarrowFOVClicked()));
    connect(ui.m_chkIR, SIGNAL(clicked()), this, SLOT(onIRClicked()));
    connect(ui.m_chkStreamView1, SIGNAL(clicked()), this, SLOT(onStreamView1Clicked()));
    connect(ui.m_cbViews1, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onViews1IndexChanged(const QString&)));
    connect(ui.m_cbControllable, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onControllableIndexChanged(const QString&)));
    connect(ui.m_chkDebug, SIGNAL(clicked()), this, SLOT(onDebugClicked()));
    connect(ui.m_chkBoundsOnly, SIGNAL(clicked()), this, SLOT(onBoundsOnlyClicked()));
    connect(ui.m_chkNormalsOnly, SIGNAL(clicked()), this, SLOT(onNormalsOnlyClicked()));
    connect(ui.m_chkDepth, SIGNAL(clicked()), this, SLOT(onDepthClicked()));
    connect(ui.m_chkOverlook, SIGNAL(clicked()), this, SLOT(onOverlookClicked()));
    connect(ui.m_chkEdit, SIGNAL(clicked()), this, SLOT(onEditClicked()));

    connect(ui.m_bReset, SIGNAL(clicked()), this, SLOT(onResetClicked()));

    loadScene(QCoreApplication::applicationDirPath() + "/" + sSceneFileName);

    m_sExportPath = QCoreApplication::applicationDirPath() + "/Export";

    if (!QDir().exists(m_sExportPath))
    {
        QDir().mkpath(m_sExportPath);
    }

    m_tPreviousTime = QDateTime::currentDateTime();

    m_tTimer.setTimerType(Qt::PreciseTimer);
    m_tTimer.start(FrameTimerPollIntervalMs);
}

//-------------------------------------------------------------------------------------------------

Quick3DTest::~Quick3DTest()
{
    // Stop background workers first : they touch scene meshes and must never outlive the scene
    CWorkerManager::killInstance();

    // Quick3DTest owns the scene explicitly : detach it from the view hierarchy
    // first, otherwise the widget tree deletes it a second time at teardown
    if (m_pScene != nullptr)
    {
        m_pScene->setParent(nullptr);
    }

    delete m_pScene;

    CComponentFactory::killInstance();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::loadScene(QString sFileName)
{
    LOG_METHOD_DEBUG("Instanciating scene...");

    m_pScene->clear();

    m_pScene->resetEnvironmentToDefaults();

    m_pScene->viewports()[0] = new CViewport(m_pScene, false);
    m_pScene->viewports()[0]->setEnabled(true);

    //-----------------------------------------------
    // Load components

    LOG_METHOD_DEBUG("Loading components...");

    QVector<QSP<CComponent> > vComponents = CComponentLoader::getInstance()->load(sFileName, m_pScene);

    m_pScene->init(vComponents);

    //-----------------------------------------------

    onResize();

    fillObjectsCombo();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QTimer::singleShot(0, this, SLOT(onResize()));
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onResize()
{
    if (m_pScene != nullptr && m_pScene->viewports().count() > 0)
    {
        m_pView->setGeometry(0, 0, ui.Render1->width(), ui.Render1->height());
        m_pScene->setGeometry(0, 0, m_pView->width(), m_pView->height());
        m_pScene->viewports()[0]->setPosition(CVector2(0.0, 0.0));
        m_pScene->viewports()[0]->setSize(CVector2(double(ui.Render1->width()), double(ui.Render1->height())));
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillObjectsCombo()
{
    m_bProcessEvents = false;

    ui.m_cbViews1->clear();
    ui.m_cbControllable->clear();

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        fillCameraCombo(pComponent.data());
    }

    ui.m_cbViews1->setCurrentIndex(-1);

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        fillControlableCombo(pComponent.data());
    }

    ui.m_cbControllable->setCurrentIndex(-1);

    m_bProcessEvents = true;
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillCameraCombo(CComponent* pComponent)
{
    if (pComponent->isCamera())
    {
        QString sQualifiedName = pComponent->qualifiedName();

        ui.m_cbViews1->addItem(sQualifiedName);
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        fillCameraCombo(pChild.data());
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillControlableCombo(CComponent* pComponent)
{
    if (pComponent->isRootObject() || pComponent->isCamera())
    {
        QString sQualifiedName = pComponent->qualifiedName();

        ui.m_cbControllable->addItem(sQualifiedName);
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        fillControlableCombo(pChild.data());
    }
}

//-------------------------------------------------------------------------------------------------

int Quick3DTest::randInt(int low, int high)
{
    return QRandomGenerator::global()->bounded(low, high + 1);
}

//-------------------------------------------------------------------------------------------------

QString Quick3DTest::getDateString() const
{
    QDateTime tTime = QDateTime::currentDateTime();

    return QString("%1-%2-%3_%4-%5-%6")
            .arg(tTime.date().year())
            .arg(tTime.date().month())
            .arg(tTime.date().day())
            .arg(tTime.time().hour())
            .arg(tTime.time().minute())
            .arg(tTime.time().second());
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTimer()
{
    m_tTimer.stop();

    if (m_pScene != nullptr && m_bRun == true)
    {
        QDateTime tCurrentTime = QDateTime::currentDateTime();
        double dDeltaTime = double(m_tPreviousTime.msecsTo(tCurrentTime)) / 1000.0;

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

        m_pScene->updateScene(dDeltaTime);
        m_pView->update(dDeltaTime);

        {
            CScopedTimeSample tUiSample(QString("Frame_SetText"));
            ui.m_lInfo->setText(m_pScene->debugInfo());
        }

        if (m_bCheckRender == true)
        {
            m_iCheckRenderFrames++;

            if (m_iCheckRenderFrames >= 600)
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

    m_tTimer.start(FrameTimerPollIntervalMs);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onLoadSceneClicked()
{
    m_bRun = false;

    QString sFileName = QFileDialog::getOpenFileName(this, "Open XML Scene", ".", tr("XML Files (*.xml)"));

    if (sFileName.isEmpty() == false)
    {
        loadScene(sFileName);
    }

    m_bRun = true;
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onClearSceneClicked()
{
    m_pScene->clear();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onExportTerrainClicked()
{
    Ui::ExportDialog ExportDialog;
    QDialog dialog;

    ExportDialog.setupUi(&dialog);

    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        CWorldTerrain* pTerrain = nullptr;

        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            if (pComponent->getClassName() == ClassName_CWorldTerrain)
            {
                pTerrain = dynamic_cast<CWorldTerrain*>(pComponent.data());

                break;
            }
        }

        if (pTerrain != nullptr)
        {
            double dStartLat = ExportDialog.m_tbStartLat->displayText().toDouble();
            double dStartLon = ExportDialog.m_tbStartLon->displayText().toDouble();
            double dSizeZ = ExportDialog.m_tbSizeZ->displayText().toDouble();
            double dSizeX = ExportDialog.m_tbSizeX->displayText().toDouble();
            double dTileSizeM = ExportDialog.m_tbTileSizeM->displayText().toDouble();
            int iPointsPerTile = ExportDialog.m_tbPointsPerTile->displayText().toInt();
            double dHeightOffset = ExportDialog.m_tbHeightOffset->displayText().toDouble();

            double dPointSpacing = dTileSizeM / double(iPointsPerTile);

            CGeoloc gStart(dStartLat, dStartLon, 0.0);

            double dCurrentX = 0.0;
            double dCurrentZ = 0.0;

            while (dCurrentZ < dSizeZ)
            {
                while (dCurrentX < dSizeX)
                {
                    QString sFilename = m_sExportPath + "/" +
                            QString("Terrain_Z%1_X%2.raw")
                            .arg(long(dCurrentZ), int(6), int(10), QChar('0'))
                            .arg(long(dCurrentX), int(6), int(10), QChar('0'));

                    QFile outputFile(sFilename);

                    double dMaxAltitude = 0.0;

                    if (outputFile.open(QIODevice::WriteOnly))
                    {
                        for (int iZ = 0; iZ < iPointsPerTile; iZ++)
                        {
                            for (int iX = 0; iX < iPointsPerTile; iX++)
                            {
                                double dPointX = dCurrentX + dPointSpacing * double(iX);
                                double dPointZ = dCurrentZ + dPointSpacing * double(iZ);

                                CGeoloc gPoint(gStart, Math::CVector3(dPointX, 0.0, dPointZ));

                                double dAltitude = dHeightOffset + pTerrain->heights()->getHeightAt(gPoint);
                                if (dAltitude < 0.0) dAltitude = 0.0;

                                if (dAltitude > dMaxAltitude) dMaxAltitude = dAltitude;

                                short sAltitude = short(dAltitude);

                                outputFile.write(reinterpret_cast<const char*>(&sAltitude), sizeof(short));
                            }
                        }

                        outputFile.close();
                    }

                    if (dMaxAltitude < 0.1)
                    {
                        QFile::remove(sFilename);
                    }

                    dCurrentX += dTileSizeM;
                }

                dCurrentZ += dTileSizeM;
                dCurrentX = 0.0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onGenerateMatrixClicked()
{
    CCamera* pCamera = nullptr;

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->isCamera())
        {
            pCamera = dynamic_cast<CCamera*>(pComponent.data());
            break;
        }
    }

    if (pCamera != nullptr)
    {
        // Dump the scene

        QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");
        if (dump.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&dump);
            m_pScene->dump(stream, 0);
            dump.close();
        }

        // Compute panoramic matrix

        QVector<CGeoZone> vZones;
        QVector<double> vDepth;
        QVector<char> vDetection;
        QVector<char> vEdges;
        QImage imgDepthImage;
        QImage imgDetectionImage;
        QImage imgContourImage;

        CPanoramicMatrixParams tParams(vZones, vDepth, vDetection, vEdges, imgDepthImage, imgDetectionImage, imgContourImage);

        tParams.m_sResolution               = QSize(600, 200);              // Resolution
        tParams.m_gCameraPosition           = pCamera->geoloc();            // Camera position
        tParams.m_dCameraTrueHeadingDegrees = 0.0;                          // True heading
        tParams.m_vStartPanTiltDegrees      = CVector2(-60.0, -180.0);      // Start tilt and pan
        tParams.m_vEndPanTiltDegrees        = CVector2(60.0, 180.0);        // End tilt and pan
        tParams.m_vPanTiltOffsetDegrees     = CVector2(0.0, 0.0);           // Tilt and pan offset

        pCamera->renderDepth_RayTraced(m_pScene, 10000.0, tParams, nullptr);

        if (vDepth.count() > 0)
        {
            QImage img(tParams.m_sResolution, QImage::Format_RGB888);

            for (int y = 0; y < tParams.m_sResolution.height(); y++)
            {
                for (int x = 0; x < tParams.m_sResolution.width(); x++)
                {
                    double dValue = vDepth[y * tParams.m_sResolution.width() + x];
                    uint uiRGB = 0;

                    double dNormValue = dValue / 20.0;
                    if (dNormValue < 0.0) dNormValue = 255.0;
                    if (dNormValue > 255.0) dNormValue = 255.0;
                    int iIntValue = int(dNormValue);

                    uiRGB = uint(iIntValue);

                    img.setPixel(x, y, uiRGB);
                }
            }

            img.save("Test.png", "PNG");
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDumpSceneClicked()
{
    // Dump of the scene

    QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");

    if (dump.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&dump);
        m_pScene->dump(stream, 0);
        dump.close();
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTimeChanged(int iValue)
{
    m_pScene->setTimeOfDay(QTime(iValue, 0, 0));
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onFogLevelChanged(int iValue)
{
    m_pScene->setFogLevel(double(iValue) / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onEnvironmentClicked()
{
    QDialog tDialog(this);
    tDialog.setWindowTitle("Environment");

    QFormLayout* pLayout = new QFormLayout(&tDialog);

    QComboBox* pSkyModel = new QComboBox(&tDialog);
    pSkyModel->addItem(QString(SkyModels::PhysicalName));
    pSkyModel->setCurrentText(m_pScene->skyModelName());
    pLayout->addRow("Sky model", pSkyModel);

    QDoubleSpinBox* pOzoneAmount = new QDoubleSpinBox(&tDialog);
    pOzoneAmount->setRange(0.0, 4.0);
    pOzoneAmount->setSingleStep(0.05);
    pOzoneAmount->setDecimals(2);
    pOzoneAmount->setValue(m_pScene->ozoneAmount());
    pLayout->addRow("Ozone amount", pOzoneAmount);

    QDoubleSpinBox* pAtmosphereAltitude = new QDoubleSpinBox(&tDialog);
    pAtmosphereAltitude->setRange(1000.0, 1000000.0);
    pAtmosphereAltitude->setSingleStep(1000.0);
    pAtmosphereAltitude->setDecimals(0);
    pAtmosphereAltitude->setValue(m_pScene->atmosphereAltitude());
    pLayout->addRow("Atmosphere altitude", pAtmosphereAltitude);

    QDoubleSpinBox* pFogMinimum = new QDoubleSpinBox(&tDialog);
    pFogMinimum->setRange(0.0, 50000.0);
    pFogMinimum->setSingleStep(10.0);
    pFogMinimum->setDecimals(0);
    pFogMinimum->setValue(m_pScene->fogMinimumDistance());
    pLayout->addRow("Fog minimum distance", pFogMinimum);

    QDoubleSpinBox* pUnderwaterFog = new QDoubleSpinBox(&tDialog);
    pUnderwaterFog->setRange(0.0, 100000.0);
    pUnderwaterFog->setSingleStep(10.0);
    pUnderwaterFog->setDecimals(0);
    pUnderwaterFog->setValue(m_pScene->underwaterFogDistance());
    pLayout->addRow("Underwater fog distance", pUnderwaterFog);

    QPushButton* pGlobalAmbientButton = new QPushButton("Choose...", &tDialog);
    pLayout->addRow("Global ambient", pGlobalAmbientButton);

    QPushButton* pSkyColorButton = new QPushButton("Choose...", &tDialog);
    pLayout->addRow("Sky color", pSkyColorButton);

    QPushButton* pFogColorButton = new QPushButton("Choose...", &tDialog);
    pLayout->addRow("Fog color", pFogColorButton);

    connect(pSkyModel, &QComboBox::currentTextChanged, [this](const QString& sName) { m_pScene->setSkyModel(sName); });
    connect(pOzoneAmount, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double dValue) { m_pScene->setOzoneAmount(dValue); });
    connect(pAtmosphereAltitude, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double dValue) { m_pScene->setAtmosphereAltitude(dValue); });
    connect(pFogMinimum, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double dValue) { m_pScene->setFogMinimumDistance(dValue); });
    connect(pUnderwaterFog, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double dValue) { m_pScene->setUnderwaterFogDistance(dValue); });

    connect(pGlobalAmbientButton, &QPushButton::clicked, [this]()
    {
        Math::CVector3 vCurrent = m_pScene->globalAmbientColor();
        QColor tChosen = QColorDialog::getColor(QColor(int(vCurrent.X * 255.0), int(vCurrent.Y * 255.0), int(vCurrent.Z * 255.0)), this, "Global ambient");

        if (tChosen.isValid() == true)
        {
            m_pScene->setGlobalAmbientColor(Math::CVector3(double(tChosen.red()) / 255.0, double(tChosen.green()) / 255.0, double(tChosen.blue()) / 255.0));
        }
    });

    connect(pSkyColorButton, &QPushButton::clicked, [this]()
    {
        Math::CVector3 vCurrent = m_pScene->skyColor();
        QColor tChosen = QColorDialog::getColor(QColor(int(vCurrent.X * 255.0), int(vCurrent.Y * 255.0), int(vCurrent.Z * 255.0)), this, "Sky color");

        if (tChosen.isValid() == true)
        {
            m_pScene->setSkyColor(Math::CVector3(double(tChosen.red()) / 255.0, double(tChosen.green()) / 255.0, double(tChosen.blue()) / 255.0));
        }
    });

    connect(pFogColorButton, &QPushButton::clicked, [this]()
    {
        Math::CVector3 vCurrent = m_pScene->fogColor();
        QColor tChosen = QColorDialog::getColor(QColor(int(vCurrent.X * 255.0), int(vCurrent.Y * 255.0), int(vCurrent.Z * 255.0)), this, "Fog color");

        if (tChosen.isValid() == true)
        {
            m_pScene->setFogColor(Math::CVector3(double(tChosen.red()) / 255.0, double(tChosen.green()) / 255.0, double(tChosen.blue()) / 255.0));
        }
    });

    tDialog.exec();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onFogColorClicked()
{
    if (m_pFogColorDialog != nullptr)
    {
        m_pFogColorDialog->raise();
        m_pFogColorDialog->activateWindow();
        return;
    }

    Math::CVector3 vCurrent = m_pScene->fogColor();

    m_pFogColorDialog = new QColorDialog(
        QColor(int(vCurrent.X * 255.0), int(vCurrent.Y * 255.0), int(vCurrent.Z * 255.0)),
        this);

    m_pFogColorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
    m_pFogColorDialog->setWindowTitle("Fog color");

    connect(m_pFogColorDialog, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(onFogColorChanged(const QColor&)));
    connect(m_pFogColorDialog, SIGNAL(finished(int)), this, SLOT(onFogColorDialogClosed()));

    m_pFogColorDialog->show();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onFogColorChanged(const QColor& tColor)
{
    if (tColor.isValid())
    {
        m_pScene->setFogColor(Math::CVector3(
            double(tColor.red()) / 255.0,
            double(tColor.green()) / 255.0,
            double(tColor.blue()) / 255.0));
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onFogColorDialogClosed()
{
    if (m_pFogColorDialog != nullptr)
    {
        m_pFogColorDialog->deleteLater();
        m_pFogColorDialog = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onWindLevelChanged(int iValue)
{
    m_pScene->setWindLevel(double(iValue) / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onShaderQualityChanged(int iValue)
{
    m_pScene->setShaderQuality(double(iValue) / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTerrainResChanged(int iValue)
{
    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->getClassName() == ClassName_CWorldTerrain)
        {
            CWorldTerrain* pTerrain = dynamic_cast<CWorldTerrain*>(pComponent.data());

            if (pTerrain != nullptr)
            {
                pTerrain->setTerrainResolution(iValue);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onMoveSpeedChanged(int iValue)
{
    if (m_pScene->controller() != nullptr)
    {
        m_pScene->controller()->setMoveSpeed(double(iValue) / 100.0);
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onOverlookFOVChanged(int iValue)
{
    m_pScene->setOverlookFOV(double(iValue));
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onRainClicked()
{
    m_pScene->setRainVisible(ui.m_chkRain->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onWideFOVClicked()
{
    m_pScene->forceWideFOV(ui.m_chkWideFOV->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onNarrowFOVClicked()
{
    m_pScene->forceSmallFOV(ui.m_chkNarrowFOV->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onIRClicked()
{
    m_pScene->forceIR(ui.m_chkIR->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onStreamView1Clicked()
{
    if (m_pScene->viewports().count() > 0)
    {
        m_pScene->viewports()[0]->setStreamView(ui.m_chkStreamView1->checkState() == Qt::Checked);
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onViews1IndexChanged(const QString& sName)
{
    if (m_bProcessEvents && m_pScene->viewports().count() > 0)
    {
        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(sName);

            if (pFound && pFound->isCamera())
            {
                m_pScene->viewports()[0]->setCamera(QSP_CAST(CCamera, pFound));
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onControllableIndexChanged(const QString& sName)
{
    if (m_bProcessEvents)
    {
        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(sName);

            if (pFound && pFound->controller() != nullptr)
            {
                m_pScene->setController(pFound->controller());
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Selects the first combo item matching sName (exact, then contains), returns its index or -1
*/
int Quick3DTest::selectComboItem(QComboBox* pCombo, const QString& sName)
{
    if (pCombo == nullptr)
    {
        return -1;
    }

    int iIndex = pCombo->findText(sName);

    if (iIndex < 0)
    {
        for (int iItem = 0; iItem < pCombo->count(); iItem++)
        {
            if (pCombo->itemText(iItem).contains(sName) == true)
            {
                iIndex = iItem;
                break;
            }
        }
    }

    if (iIndex >= 0)
    {
        pCombo->setCurrentIndex(iIndex);
    }

    return iIndex;
}

//-------------------------------------------------------------------------------------------------

/*!
    Selects a vehicle through the controllable combo box, like a user click, returns true on success
*/
bool Quick3DTest::selectVehicleByName(const QString& sName)
{
    if (m_pScene == nullptr)
    {
        return false;
    }

    if (selectComboItem(ui.m_cbControllable, sName) < 0)
    {
        return false;
    }

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        QSP<CComponent> pFound = pComponent->findComponent(sName);

        if (pFound != nullptr && pFound->controller() != nullptr)
        {
            return m_pScene->controller() == pFound->controller();
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Selects a camera through the views combo box, like a user click, returns true on success
*/
bool Quick3DTest::selectViewByName(const QString& sName)
{
    if (m_pScene == nullptr || m_pScene->viewports().count() == 0)
    {
        return false;
    }

    if (selectComboItem(ui.m_cbViews1, sName) < 0)
    {
        return false;
    }

    QSP<CCamera> pCamera = m_pScene->viewports()[0]->camera();

    return pCamera != nullptr && pCamera->qualifiedName().contains(sName) == true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Posts a key event (Qt::Key value, 0 = press, 1 = release, 2 = click) to the 3D view, returns true on success
*/
bool Quick3DTest::postKeyToView(int iKey, int iAction)
{
    if (m_pView == nullptr)
    {
        return false;
    }

    if (iAction == 0 || iAction == 2)
    {
        QApplication::postEvent(m_pView, new QKeyEvent(QEvent::KeyPress, iKey, Qt::NoModifier));
    }

    if (iAction == 1 || iAction == 2)
    {
        QApplication::postEvent(m_pView, new QKeyEvent(QEvent::KeyRelease, iKey, Qt::NoModifier));
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a JSON document describing the vehicle named sName
*/
QString Quick3DTest::vehicleStatusJson(const QString& sName)
{
    if (m_pScene == nullptr)
    {
        return "{\"ok\":false,\"error\":\"no scene\"}";
    }

    QSP<CVehicle> pVehicle;

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        QSP<CComponent> pFound = pComponent->findComponent(sName);

        if (pFound != nullptr)
        {
            pVehicle = QSP_CAST(CVehicle, pFound);
            break;
        }
    }

    if (pVehicle == nullptr)
    {
        return "{\"ok\":false,\"error\":\"vehicle not found\"}";
    }

    CGeoloc gGeoloc = pVehicle->geoloc();
    CVector3 vRotation = pVehicle->rotation();

    int iContacts = -1;

    CPhysicsWorld* pWorld = m_pScene->physicsWorld();

    if (pWorld != nullptr && pVehicle->hasPhysicsVehicle() == true)
    {
        iContacts = 0;

        QVector<QSP<CWheel> > vWheels = pVehicle->wheels();

        for (int iWheel = 0; iWheel < vWheels.count(); iWheel++)
        {
            if (pWorld->vehicleWheelHasContact(pVehicle->physicsVehicleIndex(), iWheel) == true)
            {
                iContacts++;
            }
        }
    }

    bool bControlled = m_pScene->controller() != nullptr
        && pVehicle->controller() != nullptr
        && m_pScene->controller() == pVehicle->controller();

    return QString("{\"ok\":true,\"name\":\"%1\",\"physics\":%2,\"latitude\":%3,\"longitude\":%4,\"altitude\":%5,"
        "\"rotationX\":%6,\"rotationY\":%7,\"rotationZ\":%8,\"handBrake\":%9,\"wheels\":%10,\"contacts\":%11,\"controlled\":%12}")
        .arg(pVehicle->name())
        .arg(pVehicle->hasPhysicsVehicle() == true ? "true" : "false")
        .arg(gGeoloc.Latitude, 0, 'g', 10).arg(gGeoloc.Longitude, 0, 'g', 10).arg(gGeoloc.Altitude, 0, 'g', 10)
        .arg(vRotation.X, 0, 'g', 10).arg(vRotation.Y, 0, 'g', 10).arg(vRotation.Z, 0, 'g', 10)
        .arg(pVehicle->isHandBrakeEngaged() == true ? "true" : "false")
        .arg(pVehicle->wheels().count())
        .arg(iContacts)
        .arg(bControlled == true ? "true" : "false");
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onResetClicked()
{
    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->getClassName() == ClassName_CTerrestrialVehicle || pComponent->getClassName() == ClassName_CSeaVehicle)
        {
            QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

            if (pVehicle != nullptr)
            {
                pVehicle->resetTrajectory();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDebugClicked()
{
    m_pScene->setDebugMode(ui.m_chkDebug->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onBoundsOnlyClicked()
{
    m_pScene->setBoundsOnly(ui.m_chkBoundsOnly->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onNormalsOnlyClicked()
{
    m_pScene->setNormalsOnly(ui.m_chkNormalsOnly->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDepthClicked()
{
    m_pScene->setDepthComputing(ui.m_chkDepth->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onOverlookClicked()
{
    m_pScene->setOverlookScene(ui.m_chkOverlook->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onEditClicked()
{
    m_pScene->setEditMode(ui.m_chkEdit->checkState() == Qt::Checked);
}
