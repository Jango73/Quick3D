
#pragma once

#include "components_a320_global.h"

//-------------------------------------------------------------------------------------------------

// Quick3D
#include "Interpolator.h"
#include "CGeoloc.h"

// Application
#include "Constants.h"
#include "CAirbusFlightComputer.h"
#include "CAirbusFlightPlan.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class COMPONENTS_A320_EXPORT CAirbusDMC : public CAirbusFlightComputer
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAirbusDMC(C3DScene* pScene);

    //!
    virtual ~CAirbusDMC();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAirbusDMC; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void updateTexture(CTexture* pTexture, double dDeltaTime);

    //!
    void updateTexture_PFD(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void updateTexture_ND(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void updateTexture_EWD(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void updateTexture_SD(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void drawVelocityBar(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void drawArtificialHorizon(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void drawAltitudeBar(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void drawFMA(QPainter* pPainter, CTexture* pTexture, double dDeltaTime);

    //!
    void drawRosace(QPainter* pPainter, CTexture* pTexture, double dDeltaTime, bool bArc);

    //!
    void drawWaypoint(QPainter* pPainter, CTexture* pTexture, double dDeltaTime, const CWaypoint& wWaypoint, const QRectF& rect, bool bIsFlight);

    //!
    void drawEngineN1Gauge(QPainter* pPainter, CTexture* pTexture, double dDeltaTime, const QRectF& rRect, double dN1);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QRectF						m_rVelocityBar;
    QRectF						m_rArtificialHorizon;
    QRectF						m_rAltitudeBar;
    QRectF						m_rFMA;
    Math::Interpolator<double>	m_iVerticalSpeedMarker;

    QRectF						m_rRosace;

    QRectF						m_rEWD_Engines;
    QRectF						m_rEWD_Flaps;
    QRectF						m_rEWD_Checklist;

    QFont						m_fMainFont;
    QPen						m_pGreenBold;
    QPen						m_pYellowBold;
    QPen						m_pWhiteDashed;
};
