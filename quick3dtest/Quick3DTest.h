
#pragma once

// Qt
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

// Application
#include "ui_Quick3DTest.h"
#include "CGLWidgetScene.h"
#include "CComponent.h"
#include "CView.h"

class Quick3DTest : public QMainWindow
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
#if QT_VERSION >= 0x050000
    Quick3DTest(QString sSceneFileName, QWidget *parent = 0);
#else
    Quick3DTest(QString sSceneFileName, QWidget *parent = 0, Qt::WFlags flags = 0);
#endif

    //! Destructor
    virtual ~Quick3DTest();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    void loadScene(QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    virtual void resizeEvent(QResizeEvent *event);

    //!
    void fillObjectsCombo();

    //!
    void fillCameraCombo(CComponent* pComponent);

    //!
    void fillControlableCombo(CComponent* pComponent);

    //!
    int randInt(int low, int high);

    //!
    QString getDateString() const;

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

private slots:

    void onTimer();
    void onLoadSceneClicked();
    void onClearSceneClicked();
    void onExportTerrainClicked();
    void onGenerateMatrixClicked();
    void onDumpSceneClicked();
    void onRainClicked();
    void onWideFOVClicked();
    void onNarrowFOVClicked();
    void onIRClicked();
    void onStreamView1Clicked();
    void onTimeChanged(int iValue);
    void onFogLevelChanged(int iValue);
    void onWindLevelChanged(int iValue);
    void onShaderQualityChanged(int iValue);
    void onTerrainResChanged(int iValue);
    void onMoveSpeedChanged(int iValue);
    void onOverlookFOVChanged(int iValue);
    void onViews1IndexChanged(const QString&);
    void onControllableIndexChanged(const QString& sName);
    void onResetClicked();
    void onDebugClicked();
    void onBoundsOnlyClicked();
    void onNormalsOnlyClicked();
    void onDepthClicked();
    void onOverlookClicked();
    void onEditClicked();
    void onResize();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Ui::Quick3DTestClass            ui;
    CGLWidgetScene*                 m_pScene;
    CView*                          m_pView;
    QTimer                          m_tTimer;
    QDateTime                       m_tPreviousTime;
    QString                         m_sExportPath;
    bool                            m_bProcessEvents;
    bool                            m_bRun;
};
