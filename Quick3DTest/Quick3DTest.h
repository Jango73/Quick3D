
#ifndef QUICK3DTEST_H
#define QUICK3DTEST_H

// QT
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
#include "CWorldTerrain.h"

class Quick3DTest : public QMainWindow
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
#if QT_VERSION >= 0x050000
    Quick3DTest(QString sSceneFileName, QWidget *parent = 0);
#else
    Quick3DTest(QString sSceneFileName, QWidget *parent = 0, Qt::WFlags flags = 0);
#endif

    //! Destructeur
    virtual ~Quick3DTest();

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    void loadScene(QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
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
    void onViews1IndexChanged(const QString&);
    void onControllableIndexChanged(const QString& sName);
    void onResetClicked();
    void onDebugClicked();
    void onBoundsOnlyClicked();
    void onNormalsOnlyClicked();
    void onDepthClicked();
    void onResize();

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    Ui::Quick3DTestClass            ui;
    CGLWidgetScene*                 m_pScene;
    CView*                          m_pView;
    QTimer                          m_tTimer;
    CAverager<double>               m_FPS;
    QDateTime                       m_tPreviousTime;
    QString                         m_sExportPath;
    bool                            m_bProcessEvents;
    bool                            m_bRun;
};

#endif // QUICK3DTEST_H
