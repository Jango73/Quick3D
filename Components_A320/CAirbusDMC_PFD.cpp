
// Qt
#include <QPainter>

// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

// Application
#include "CAirbusDMC.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::updateTexture_PFD(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    drawVelocityBar(pPainter, pTexture, dDeltaTime);
    drawArtificialHorizon(pPainter, pTexture, dDeltaTime);
    drawAltitudeBar(pPainter, pTexture, dDeltaTime);
    drawFMA(pPainter, pTexture, dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawVelocityBar(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    // Get flight data
    CAirbusData* pAirspeed_ms = getData(adAir_IndicatedAirspeed_ms);
    CAirbusData* pAirspeedVMax_ms = getData(adAir_IndicatedAirspeedVMax_ms);

    double dAirspeed_ms = 0.0;
    double dAirspeed_kts = 0.0;
    double dAirspeed_VMax_ms = 0.0;
    double dAirspeed_VMax1_kts = 0.0;
    double dAirspeed_VMax2_kts = 0.0;

    if (pAirspeed_ms != NULL)
    {
        dAirspeed_ms = pAirspeed_ms->getData().toDouble();
        dAirspeed_kts = dAirspeed_ms * FAC_MS_TO_KNOTS;
    }

    if (pAirspeedVMax_ms != NULL)
    {
        dAirspeed_VMax_ms = pAirspeedVMax_ms->getData().toDouble();
        dAirspeed_VMax1_kts = dAirspeed_VMax_ms * FAC_MS_TO_KNOTS;
        dAirspeed_VMax2_kts = dAirspeed_VMax1_kts * 2.0;
    }

    // Compute nearest 10th knots
    int iNearestTenKnots = ( ((int) (dAirspeed_kts / 10.0) ) * 10);

    // Compute coordinates
    double X = m_rVelocityBar.left() * pTexture->getImage().width();
    double Y = m_rVelocityBar.top() * pTexture->getImage().height();
    double W = m_rVelocityBar.width() * pTexture->getImage().width();
    double H = m_rVelocityBar.height() * pTexture->getImage().height();
    double W2 = W / 2.0;
    double W4 = W / 4.0;
    double W10 = W / 10.0;
    // double H2 = H / 2.0;
    double dVelocityScale = 0.1;

    QRectF rWholePart(X, Y, W, H);
    QRectF rLeftPart(X, Y, W2 + W4, H);
    QRectF rRightPart(X + rLeftPart.width(), Y, rWholePart.width() - rLeftPart.width(), H);

    QPointF vLeftPartCenter(
                rLeftPart.left() + rLeftPart.width() / 2.0,
                rLeftPart.top() + rLeftPart.height() / 2.0
                );

    QPointF vRightPartCenter(
                rRightPart.left() + rRightPart.width() / 2.0,
                rRightPart.top() + rRightPart.height() / 2.0
                );

    double LW = rLeftPart.width();
    double LH = rLeftPart.height();
    double LW2 = LW / 2.0;
    double LH2 = LH / 2.0;

    double RW = rRightPart.width();
    // double RH = rRightPart.height();
    double RW2 = RW / 2.0;
    // double RH2 = RH / 2.0;

    // Create clipping path and prepare painter
    QPainterPath pPath;
    pPath.addRect(rLeftPart);

    pPainter->resetTransform();
    pPainter->setClipPath(pPath);
    pPainter->setClipping(true);

    // Set main font
    pPainter->setFont(m_fMainFont);

    // Velocity background
    pPainter->setPen(A320_Color_White);
    pPainter->fillRect(rLeftPart, A320_Color_DarkGray);

    pPainter->save();
    pPainter->translate(vLeftPartCenter);

    // Knots bars
    for (int iVelocityStep = -40; iVelocityStep < 40; iVelocityStep += 5)
    {
        int iVelocity = iNearestTenKnots + iVelocityStep;
        double dVelocityPos = ((dAirspeed_kts - (double) iVelocity) / dVelocityScale);
        QString sVelocity = QString::number(iVelocity);

        pPainter->drawLine(QPointF(LW2 - W10, dVelocityPos), QPointF(LW2, dVelocityPos));

        if (iVelocity % 10 == 0)
        {
            pPainter->drawText(QRectF(-LW2, dVelocityPos - LW2, LW, LW), Qt::AlignCenter, sVelocity);
        }
    }

    pPainter->restore();
    pPainter->setClipping(false);

    pPainter->drawLine(rLeftPart.topLeft(), rLeftPart.topLeft() + QPointF(rWholePart.width(), 0.0));
    pPainter->drawLine(rLeftPart.bottomLeft(), rLeftPart.bottomLeft() + QPointF(rWholePart.width(), 0.0));
    pPainter->drawLine(rLeftPart.topRight(), rLeftPart.bottomRight());

    pPainter->setPen(A320_Color_Yellow);

    pPainter->drawLine(rLeftPart.topLeft() + QPointF(0.0, LH2), rLeftPart.topRight() + QPointF(0.0, LH2));

    //-----------------------------------------------

    // Right part
    pPath = QPainterPath();
    pPath.addRect(rRightPart);

    pPainter->resetTransform();
    pPainter->setClipPath(pPath);
    pPainter->setClipping(true);
    pPainter->save();
    pPainter->translate(vRightPartCenter);

    double dVMaxPos1 = ((dAirspeed_kts - dAirspeed_VMax1_kts) / dVelocityScale);
    double dVMaxPos2 = ((dAirspeed_kts - dAirspeed_VMax2_kts) / dVelocityScale);

    QRectF rVMax(-RW2, dVMaxPos2, RW2, dVMaxPos1 - dVMaxPos2);

    pPainter->fillRect(rVMax, A320_Color_Red);

    pPainter->restore();
    pPainter->setClipping(false);
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawArtificialHorizon(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    // Get flight data
    CAirbusData* pPitch_deg = getData(adInertial_Pitch_deg);
    CAirbusData* pRoll_deg = getData(adInertial_Roll_deg);

    double dAircraftPitch = 0.0;
    double dAircraftRoll = 0.0;

    if (pPitch_deg != NULL)
    {
        dAircraftPitch = pPitch_deg->getData().toDouble();
    }

    if (pRoll_deg != NULL)
    {
        dAircraftRoll = pRoll_deg->getData().toDouble();
    }

    // Compute coordinates
    double X = m_rArtificialHorizon.left() * pTexture->getImage().width();
    double Y = m_rArtificialHorizon.top() * pTexture->getImage().height();
    double W = m_rArtificialHorizon.width() * pTexture->getImage().width();
    double H = m_rArtificialHorizon.height() * pTexture->getImage().height();
    double W2 = W * 0.50;
    double W3 = W * 0.30;
    double H2 = H * 0.50;
    double H4 = H * 0.25;
    double dDegreeScale = 50.0;

    QPointF vCenter(X + W2, Y + H2);

    // Create clipping path and prepare painter
    QPainterPath pArtificialHorizonPath = QPainterPath(QPointF(W2, -H4));
    pArtificialHorizonPath.cubicTo(QPointF(W3, -H2), QPointF(-W3, -H2), QPointF(-W2, -H4));
    pArtificialHorizonPath.lineTo(QPointF(-W2, H4));
    pArtificialHorizonPath.cubicTo(QPointF(-W3, H2), QPointF(W3, H2), QPointF(W2, H4));
    pArtificialHorizonPath.lineTo(QPointF(W2, -H4));

    pPainter->resetTransform();
    pPainter->translate(vCenter);
    pPainter->setClipPath(pArtificialHorizonPath);
    pPainter->setClipping(true);

    // Set main font
    pPainter->setFont(m_fMainFont);

    // Set transform for rotating stuff
    pPainter->rotate(dAircraftRoll);
    pPainter->translate(QPointF(0.0, (-dAircraftPitch / dDegreeScale) * H));

    // Ground/air background and horizon line
    pPainter->setPen(A320_Color_White);
    pPainter->fillRect(-W,  -H * 2, W * 2, H * 2, A320_Color_Sky);
    pPainter->fillRect(-W, 0.0, W * 2, H * 2, A320_Color_Ground);
    pPainter->drawLine(QPointF(-1.0 * W, 0.0), QPointF(1.0 * W, 0.0));

    // Large pitch lines (every 10 degrees)
    for (int iPitch = -90; iPitch <= 90; iPitch += 10)
    {
        double dPitch = ((double) iPitch / dDegreeScale) * H;
        QString sPitch = QString::number(abs(iPitch));

        pPainter->drawLine(QPointF(-0.1 * W, dPitch), QPointF(0.1 * W, dPitch));

        if (iPitch != 0)
        {
            pPainter->drawText(QRectF(-0.3 * W, dPitch - 0.05 * W, 0.1 * W, 0.1 * W), Qt::AlignCenter, sPitch);
            pPainter->drawText(QRectF( 0.2 * W, dPitch - 0.05 * W, 0.1 * W, 0.1 * W), Qt::AlignCenter, sPitch);
        }
    }

    // Smaller pitch lines (every 5 degrees)
    for (int iPitch = -85; iPitch <= 85; iPitch += 10)
    {
        double dPitch = ((double) iPitch / dDegreeScale) * H;

        pPainter->drawLine(QPointF(-0.05 * W, dPitch), QPointF(0.05 * W, dPitch));
    }

    pPainter->setClipping(false);
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawAltitudeBar(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    // Get flight data
    CAirbusData* pAltitude_m = getData(adAir_Altitude_m);
    CAirbusData* pVerticalSpeed_ms = getData(adAir_VerticalSpeed_ms);

    double dAircraftAltitude_m = 0.0;
    double dAircraftAltitude_f = 0.0;
    double dAircraftVerticalSpeed_ms = 0.0;

    if (pAltitude_m != NULL)
    {
        dAircraftAltitude_m = pAltitude_m->getData().toDouble();
        dAircraftAltitude_f = dAircraftAltitude_m * FAC_METERS_TO_FEET;
    }

    if (pVerticalSpeed_ms != NULL)
    {
        dAircraftVerticalSpeed_ms = pVerticalSpeed_ms->getData().toDouble();
    }

    // Compute nearest flight level
    int iNearestFlightLevel = ( ((int) (dAircraftAltitude_f / 100.0) ) * 100);

    // Compute coordinates
    double X = m_rAltitudeBar.left() * pTexture->getImage().width();
    double Y = m_rAltitudeBar.top() * pTexture->getImage().height();
    double W = m_rAltitudeBar.width() * pTexture->getImage().width();
    double H = m_rAltitudeBar.height() * pTexture->getImage().height();
    double W2 = W / 2.00;
    // double W3 = W / 3.00;
    // double W4 = W / 4.00;
    // double H2 = H * 0.50;
    double dAltitudeScale = 4.0;

    QRectF rWholePart(X, Y, W, H);
    QRectF rLeftPart(X, Y, W2, H);
    QRectF rRightPart(X + rLeftPart.width(), Y, rWholePart.width() - rLeftPart.width(), H);

    QPointF vLeftPartCenter(
                rLeftPart.left() + rLeftPart.width() / 2.0,
                rLeftPart.top() + rLeftPart.height() / 2.0
                );

    QPointF vRightPartCenter(
                rRightPart.left() + rRightPart.width() / 2.0,
                rRightPart.top() + rRightPart.height() / 2.0
                );

    // Right part

    QPainterPath pRightPath;
    pRightPath.addRect(rRightPart);

    double RW = rRightPart.width();
    double RH = rRightPart.height();
    double RW2 = RW / 2.0;
    // double RW4 = RW / 4.0;
    double RW10 = RW / 10.0;
    double RH2 = RH / 2.0;
    double RH4 = RH / 4.0;

    pPainter->resetTransform();
    pPainter->setClipPath(pRightPath);
    pPainter->setClipping(true);

    // Vertical speed background

    QRectF rRightPartRight(rRightPart.left() + RW2, rRightPart.top(), rRightPart.width(), rRightPart.height());

    pPainter->fillRect(rRightPartRight, A320_Color_DarkGray);

    pPainter->save();
    pPainter->translate(vRightPartCenter);

    pPainter->setPen(m_pGreenBold);

    // Vertical speed marks

    for (double dVSPosition = -2000; dVSPosition < 2500.0; dVSPosition += 500.0)
    {
        double dY = m_iVerticalSpeedMarker.getValue(dVSPosition * FAC_FPM_TO_MS);

        pPainter->drawLine(QPointF(-RW10, dY * RH2), QPointF(0.0, dY * RH2));
    }

    // Vertical speed line

    double dVSPosition = m_iVerticalSpeedMarker.getValue(dAircraftVerticalSpeed_ms);

    pPainter->drawLine(QPointF(0.0, dVSPosition * RH2), QPointF(RW2, dVSPosition * RH4));

    pPainter->restore();
    pPainter->setClipping(false);

    // Left part
    QPainterPath pPath;
    pPath.addRect(rLeftPart);

    double LW = rLeftPart.width();
    // double LH = rLeftPart.height();
    double LW2 = LW / 2.0;
    // double LW3 = LW / 3.0;
    double LW4 = LW / 4.0;
    double LW10 = LW / 10.0;
    // double LH2 = LH / 2.0;

    pPainter->resetTransform();
    pPainter->setClipPath(pPath);
    pPainter->setClipping(true);

    // Set main font
    pPainter->setFont(m_fMainFont);

    // Altitude background
    pPainter->setPen(A320_Color_White);
    pPainter->fillRect(rLeftPart, A320_Color_DarkGray);

    pPainter->save();
    pPainter->translate(vLeftPartCenter);

    // Flight level bars
    for (int iAltitude = -1000; iAltitude < 1000; iAltitude += 100)
    {
        int iFlightLevel = iNearestFlightLevel + iAltitude;
        double dFlightLevelPos = ((dAircraftAltitude_f - (double) iFlightLevel) / dAltitudeScale);
        QString sFlightLevel = QString::number((iFlightLevel) / 100);

        pPainter->drawLine(QPointF(LW2 - LW10, dFlightLevelPos), QPointF(LW2, dFlightLevelPos));

        if (iFlightLevel % 500 == 0)
        {
            pPainter->drawText(QRectF(-LW2, dFlightLevelPos - LW4, LW, LW2), Qt::AlignCenter, sFlightLevel);
        }
    }

    pPainter->restore();
    pPainter->setClipping(false);

    pPainter->drawLine(rLeftPart.topLeft(), rLeftPart.topRight());
    pPainter->drawLine(rLeftPart.bottomLeft(), rLeftPart.bottomRight());
    pPainter->drawLine(rLeftPart.topRight(), rLeftPart.bottomRight());
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawFMA(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    CAirbusData* pFCU_AutoPilot1_Engaged = getData(adFCU_AutoPilot1_Engaged);
    CAirbusData* pFCU_AutoPilot2_Engaged = getData(adFCU_AutoPilot2_Engaged);
    CAirbusData* pFCU_AutoThrust_Engaged = getData(adFCU_AutoThrust_Engaged);
    CAirbusData* pFG_LateralMode_alm = getData(adFG_LateralMode_alm);
    CAirbusData* pFG_VerticalMode_avm = getData(adFG_VerticalMode_avm);

    bool bFCU_AutoPilot1_Engaged = false;
    bool bFCU_AutoPilot2_Engaged = false;
    bool bFCU_AutoThrust_Engaged = false;
    EAirbusLateralMode eFG_LateralMode_alm = almNone;
    EAirbusVerticalMode eFG_VerticalMode_avm = avmNone;

    if (pFCU_AutoPilot1_Engaged != NULL)
    {
        bFCU_AutoPilot1_Engaged = pFCU_AutoPilot1_Engaged->getData().toBool();
    }

    if (pFCU_AutoPilot2_Engaged != NULL)
    {
        bFCU_AutoPilot2_Engaged = pFCU_AutoPilot2_Engaged->getData().toBool();
    }

    if (pFCU_AutoThrust_Engaged != NULL)
    {
        bFCU_AutoThrust_Engaged = pFCU_AutoThrust_Engaged->getData().toBool();
    }

    if (pFG_LateralMode_alm != NULL)
    {
        eFG_LateralMode_alm = (EAirbusLateralMode) pFG_LateralMode_alm->getData().toInt();
    }

    if (pFG_VerticalMode_avm != NULL)
    {
        eFG_VerticalMode_avm = (EAirbusVerticalMode) pFG_VerticalMode_avm->getData().toInt();
    }

    // Compute coordinates
    double X = m_rFMA.left() * pTexture->getImage().width();
    double Y = m_rFMA.top() * pTexture->getImage().height();
    double W = m_rFMA.width() * pTexture->getImage().width();
    double H = m_rFMA.height() * pTexture->getImage().height();
    double W5 = W / 5.0;
    double H3 = H / 3.0;

    QRectF rPart1_1(X + W5 * 0, Y + H3 * 0, W5, H3);	// FMA Col 1 Row 1
    QRectF rPart2_1(X + W5 * 1, Y + H3 * 0, W5, H3);	// FMA Col 2 Row 1
    QRectF rPart3_1(X + W5 * 2, Y + H3 * 0, W5, H3);	// FMA Col 3 Row 1
    QRectF rPart4_1(X + W5 * 3, Y + H3 * 0, W5, H3);	// FMA Col 4 Row 1
    QRectF rPart5_1(X + W5 * 4, Y + H3 * 0, W5, H3);	// FMA Col 5 Row 1
    QRectF rPart5_2(X + W5 * 4, Y + H3 * 1, W5, H3);	// FMA Col 5 Row 2
    QRectF rPart5_3(X + W5 * 4, Y + H3 * 2, W5, H3);	// FMA Col 5 Row 3

    QString sTextPart1_1;
    QString sTextPart2_1;
    QString sTextPart3_1;
    QString sTextPart4_1;
    QString sTextPart5_1;
    QString sTextPart5_2;
    QString sTextPart5_3;

    if (bFCU_AutoPilot1_Engaged || bFCU_AutoPilot2_Engaged)
    {
        if (bFCU_AutoPilot1_Engaged)
        {
            sTextPart5_1 = "AP1";
        }
        else if (bFCU_AutoPilot2_Engaged)
        {
            sTextPart5_1 = "AP2";
        }

        switch (eFG_LateralMode_alm)
        {
            case almNone :
                break;
            case almHeading :
                sTextPart3_1 = "HDG";
                break;
            case almNav :
                sTextPart3_1 = "NAV";
                break;
        }

        switch (eFG_VerticalMode_avm)
        {
            case avmVerticalSpeedHold :
                sTextPart2_1 = "VS";
                break;
            case avmAltitudeHold :
                sTextPart2_1 = "ALT";
                break;
            case avmNav :
                sTextPart2_1 = "NAV";
                break;
            case avmOpenClimb :
                sTextPart2_1 = "OP CLB";
                break;
            case avmOpenDescent :
                sTextPart2_1 = "OP DES";
                break;
            case avmClimb :
                sTextPart2_1 = "CLB";
                break;
            case avmDescent :
                sTextPart2_1 = "DES";
                break;
        }
    }

    if (bFCU_AutoThrust_Engaged)
    {
        sTextPart1_1 = "SPEED";
        sTextPart5_3 = "A/THR";
    }

    // Col 1
    pPainter->setPen(A320_Color_Green);
    pPainter->drawText(rPart1_1, Qt::AlignCenter, sTextPart1_1);

    // Col 2
    pPainter->setPen(A320_Color_Green);
    pPainter->drawText(rPart2_1, Qt::AlignCenter, sTextPart2_1);

    // Col 3
    pPainter->setPen(A320_Color_Green);
    pPainter->drawText(rPart3_1, Qt::AlignCenter, sTextPart3_1);

    // Col 5
    pPainter->setPen(A320_Color_White);
    pPainter->drawText(rPart5_1, Qt::AlignCenter, sTextPart5_1);
    pPainter->drawText(rPart5_2, Qt::AlignCenter, sTextPart5_2);
    pPainter->drawText(rPart5_3, Qt::AlignCenter, sTextPart5_3);
}
