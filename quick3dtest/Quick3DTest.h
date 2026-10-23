
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
#include "CColorSelector.h"
#include "CComponent.h"
#include "CView.h"

class CRemoteControlServer;
class CUiRemoteControlHandler;

class Quick3DTest : public QMainWindow
{
    Q_OBJECT

public:
    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
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

    //! Enables render check mode : captures the 3D view after a few frames, logs the black pixel ratio, quits
    void setCheckRenderMode(bool bEnabled) { m_bCheckRender = bEnabled; }

    //! Selects a vehicle through the controllable combo box, like a user click, returns true on success
    bool selectVehicleByName(const QString& sName);

    //! Selects a camera through the views combo box, like a user click, returns true on success
    bool selectViewByName(const QString& sName);

    //! Posts a key event (Qt::Key value, 0 = press, 1 = release, 2 = click) to the 3D view, returns true on success
    bool postKeyToView(int iKey, int iAction);

    //! Sets the move speed slider like a user drag, returns true on success
    bool setMoveSpeedSlider(int iValue);

    //! Returns a JSON document describing the vehicle named sName
    QString vehicleStatusJson(const QString& sName);

    //! Returns a JSON document with the full engine state for the current frame
    QString engineSnapshotJson();

    //-------------------------------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------------------------------

    //! Folder holding engine snapshots, resolved against the binary folder like scenes and models
    static constexpr const char* SnapshotsFolderName = "snapshots";

    //! Snapshot file name without extension: snapshot-YYYYMMDD-hhmmss-f<frame>
    static constexpr const char* SnapshotFilePattern = "snapshot-%1-f%2";

    //! Snapshot file name timestamp format
    static constexpr const char* SnapshotStampFormat = "yyyyMMdd-hhmmss";

    //! Snapshot file name attempts before giving up on a unique name
    static constexpr int MaxSnapshotAttempts = 1000;

    //! Frame timer poll interval in milliseconds
    static constexpr int FrameTimerPollIntervalMs = 5;

    //! Maximum frame rate in frames per second
    static constexpr double MaximumFrameRateFps = 60.0;

    //! Minimum frame duration in seconds, derived from the maximum frame rate
    static constexpr double MinimumFrameDurationS = 1.0 / MaximumFrameRateFps;

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

    //! Selects the first combo item matching sName (exact, then contains), returns its index or -1
    int selectComboItem(QComboBox* pCombo, const QString& sName);

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
    void onEnvironmentClicked();
    void onFogColorClicked();
    void onWindLevelChanged(int iValue);
    void onShaderQualityChanged(int iValue);
    void onTerrainResChanged(int iValue);
    void onMoveSpeedChanged(int iValue);
    void onOverlookFOVChanged(int iValue);
    void onViews1IndexChanged(const QString&);
    void onControllableIndexChanged(const QString& sName);
    void onResetClicked();
    void onPauseToggled(bool bChecked);
    void onStepClicked();
    void onSnapshotClicked();
    void onDebugClicked();
    void onBoundsOnlyClicked();
    void onNormalsOnlyClicked();
    void onDepthClicked();
    void onOverlookClicked();
    void onEditClicked();
    void onLightRaysClicked();
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
    bool                            m_bCheckRender;
    int                             m_iCheckRenderFrames;
    int                             m_iFrames;
    int                             m_iSingleSteps;
    CUiRemoteControlHandler*        m_pRemoteHandler;
    CRemoteControlServer*           m_pRemoteServer;
    CColorSelector                  m_tColorSelector;
};
