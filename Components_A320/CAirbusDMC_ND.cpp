
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

void CAirbusDMC::updateTexture_ND(QPainter* pPainter, CTexture* pTexture, double dDeltaTime)
{
    drawRosace(pPainter, pTexture, dDeltaTime, true);
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawRosace(QPainter* pPainter, CTexture* pTexture, double dDeltaTime, bool bArc)
{
    // Get flight data
    CAirbusData* pGeoLoc_Latitude_deg = getData(adGeoLoc_Latitude_deg);
    CAirbusData* pGeoLoc_Longitude_deg = getData(adGeoLoc_Longitude_deg);
    CAirbusData* pGeoLoc_TrueHeading_deg = getData(adGeoLoc_TrueHeading_deg);
    CAirbusData* pGeoLoc_TrueTrack_deg = getData(adGeoLoc_TrueTrack_deg);
    CAirbusData* pFCU_Heading_deg = getData(adFCU_Heading_deg);
    CAirbusData* pFG_FlightPlan_ptr = getData(adFG_FlightPlan_ptr);

    double dGeoLoc_Latitude_deg = 0.0;
    double dGeoLoc_Longitude_deg = 0.0;
    double dGeoLoc_TrueHeading_deg = 0.0;
    double dGeoLoc_TrueTrack_deg = 0.0;
    double dFCU_Heading_deg = 0.0;
    CAirbusFlightPlan* pFG_FlightPlan = NULL;

    if (pGeoLoc_Latitude_deg != NULL)
    {
        dGeoLoc_Latitude_deg = pGeoLoc_Latitude_deg->getData().toDouble();
    }

    if (pGeoLoc_Longitude_deg != NULL)
    {
        dGeoLoc_Longitude_deg = pGeoLoc_Longitude_deg->getData().toDouble();
    }

    if (pGeoLoc_TrueHeading_deg != NULL)
    {
        dGeoLoc_TrueHeading_deg = pGeoLoc_TrueHeading_deg->getData().toDouble();
    }

    if (pGeoLoc_TrueTrack_deg != NULL)
    {
        dGeoLoc_TrueTrack_deg = pGeoLoc_TrueTrack_deg->getData().toDouble();
    }

    if (pFCU_Heading_deg != NULL)
    {
        dFCU_Heading_deg = pFCU_Heading_deg->getData().toDouble();
    }

    if (pFG_FlightPlan_ptr != NULL)
    {
        pFG_FlightPlan = (CAirbusFlightPlan*) pFG_FlightPlan_ptr->getData().toULongLong();
    }

    //-----------------------------------------------------------------------------

    double dRange_m = 40 * FAC_NM_TO_M;

    // Compute coordinates
    double X = m_rRosace.left() * pTexture->getImage().width();
    double Y = m_rRosace.top() * pTexture->getImage().height();
    double W = m_rRosace.width() * pTexture->getImage().width();
    double H = m_rRosace.height() * pTexture->getImage().height();

    double W8 = W / 8.0;
    double W10 = W / 10.0;
    double W20 = W / 20.0;
    double W30 = W / 30.0;
    double H2 = H / 2.0;
    double H8 = H / 8.0;
    double H20 = H / 20.0;
    double H40 = H / 40.0;

    if (bArc)
    {
        X = (m_rRosace.center().x() - m_rRosace.width()) * pTexture->getImage().width();
        Y = m_rRosace.top() * pTexture->getImage().height();
        W = (m_rRosace.width() * 2.0) * pTexture->getImage().width();
        H = (m_rRosace.height() * 2.0) * pTexture->getImage().height();

        W8 = W / 16.0;
        W20 = W / 40.0;
        W30 = W / 60.0;
        H2 = H / 4.0;
        H8 = H / 16.0;
        H20 = H / 40.0;
        H40 = H / 80.0;
    }

    double dRangeFactor = (W * 0.5) / dRange_m;

    QRectF rWholePart(X, Y, W, H);

    // Set main font
    pPainter->setFont(m_fMainFont);

    pPainter->resetTransform();

    // Main ellipses
    pPainter->setPen(A320_Color_White);

    pPainter->drawEllipse(rWholePart);

    pPainter->setPen(m_pWhiteDashed);

    pPainter->save();
    pPainter->translate(rWholePart.center() * 0.25);
    pPainter->scale(0.75, 0.75);
    pPainter->drawEllipse(rWholePart);
    pPainter->restore();

    pPainter->save();
    pPainter->translate(rWholePart.center() * 0.5);
    pPainter->scale(0.50, 0.50);
    pPainter->drawEllipse(rWholePart);
    pPainter->restore();

    pPainter->save();
    pPainter->translate(rWholePart.center() * 0.75);
    pPainter->scale(0.25, 0.25);
    pPainter->drawEllipse(rWholePart);
    pPainter->restore();

    pPainter->translate(rWholePart.center());

    // Flight plan
    if (pFG_FlightPlan != NULL)
    {
        CMatrix4 mHeading = CMatrix4::MakeRotation(CVector3(0.0, Angles::toRad(-dGeoLoc_TrueHeading_deg), 0.0));
        CGeoloc gGeoloc(Angles::toRad(dGeoLoc_Latitude_deg), Angles::toRad(dGeoLoc_Longitude_deg), 0.0);
        CVector3 vPreviousPosition;

        for (int iIndex = 0; iIndex < pFG_FlightPlan->getWaypoints().count(); iIndex++)
        {
            CVector3 vCurrentPosition = pFG_FlightPlan->getWaypoints()[iIndex].getGeoloc().toVector3(gGeoloc);
            vCurrentPosition = mHeading * vCurrentPosition;
            vCurrentPosition.Z *= -1.0;
            vCurrentPosition *= dRangeFactor;

            QRectF wpt(QPointF(vCurrentPosition.X - W30, vCurrentPosition.Z - W30), QSizeF(W30 * 2, W30 * 2));

            if (iIndex > 0)
            {
                pPainter->drawLine(vPreviousPosition.X, vPreviousPosition.Z, vCurrentPosition.X, vCurrentPosition.Z);
            }

            drawWaypoint(pPainter, pTexture, dDeltaTime, pFG_FlightPlan->getWaypoints()[iIndex], wpt, true);

            vPreviousPosition = vCurrentPosition;
        }
    }

    // Aircraft

    {
        QPointF point1(0.0, -W20);
        QPointF point2(0.0, W10);
        QPointF point3(-W20, 0.0);
        QPointF point4(W20, 0.0);

        pPainter->setPen(m_pYellowBold);
        pPainter->drawLine(point1, point2);
        pPainter->drawLine(point3, point4);
    }

    pPainter->rotate(-dGeoLoc_TrueHeading_deg);

    pPainter->setPen(A320_Color_White);

    // Marks
    pPainter->save();

    for (int iAngle = 0; iAngle < 360; iAngle += 10)
    {
        pPainter->drawLine(0.0, rWholePart.height() * -0.5, 0.0, rWholePart.height() * -0.5 - H20);
        pPainter->rotate(10);
    }

    pPainter->restore();

    pPainter->save();

    pPainter->rotate(5);
    for (int iAngle = 0; iAngle < 360; iAngle += 10)
    {
        pPainter->drawLine(0.0, rWholePart.height() * -0.5, 0.0, rWholePart.height() * -0.5 - H40);
        pPainter->rotate(10);
    }

    pPainter->restore();

    // Numbers

    {
        pPainter->save();

        for (int iAngle = 0; iAngle < 360; iAngle += 30)
        {
            QRectF rText(-W8, rWholePart.height() * -0.5 - H8 * 2.0, W8 * 2.0, H8 * 2.0);
            pPainter->drawText(rText, Qt::AlignCenter, QString::number(iAngle / 10));
            pPainter->rotate(30);
        }

        pPainter->restore();
    }

    // Selected heading

    {
        pPainter->save();

        pPainter->rotate(dFCU_Heading_deg);

        pPainter->setPen(A320_Color_Blue);

        QPointF point1(0.0, rWholePart.height() * -0.5);
        QPointF point2(-W20, rWholePart.height() * -0.5 - W20);
        QPointF point3(W20, rWholePart.height() * -0.5 - W20);

        pPainter->drawLine(point1, point2);
        pPainter->drawLine(point2, point3);
        pPainter->drawLine(point3, point1);

        pPainter->restore();
    }

    // Track

    {
        pPainter->save();

        pPainter->rotate(dGeoLoc_TrueTrack_deg - dGeoLoc_TrueHeading_deg);

        pPainter->setPen(A320_Color_Green);

        QPointF point1(0.0, rWholePart.height() * -0.5);
        QPointF point2(-H40, rWholePart.height() * -0.5 + H40);
        QPointF point3(0.0, rWholePart.height() * -0.5 + H40 * 2.0);
        QPointF point4(H40, rWholePart.height() * -0.5 + H40);

        pPainter->drawLine(point1, point2);
        pPainter->drawLine(point2, point3);
        pPainter->drawLine(point3, point4);
        pPainter->drawLine(point4, point1);

        pPainter->restore();
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusDMC::drawWaypoint(QPainter* pPainter, CTexture* pTexture, double dDeltaTime, const CWaypoint& wWaypoint, const QRectF& rect, bool bIsFlight)
{
    double dX1 = rect.left();
    double dY1 = rect.top();
    double dX2 = rect.right();
    double dY2 = rect.bottom();
    double dXC = rect.center().x();
    double dYC = rect.center().y();

    if (bIsFlight)
    {
        pPainter->setPen(A320_Color_Green);
        pPainter->drawLine(dXC, dY1, dX2, dYC);
        pPainter->drawLine(dX2, dYC, dXC, dY2);
        pPainter->drawLine(dXC, dY2, dX1, dYC);
        pPainter->drawLine(dX1, dYC, dXC, dY1);
    }
    else
    {
        switch (wWaypoint.getType())
        {
            case wtAirport:
                pPainter->setPen(A320_Color_Purple);
                // pPainter->drawEllipse(wpt);
                pPainter->drawLine(dX1, dYC, dX2, dYC);
                pPainter->drawLine(dXC, dY1, dXC, dY2);
                pPainter->drawLine(dX1, dY1, dX2, dY2);
                pPainter->drawLine(dX1, dY2, dX2, dY1);
                break;
        }
    }

    QRectF rText(dX2, dYC, rect.width() * 4.0, rect.height());
    pPainter->drawText(rText, Qt::AlignLeft, wWaypoint.getName());
}
