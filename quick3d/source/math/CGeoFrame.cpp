
// Application
#include "CGeoFrame.h"
#include "CAxis.h"

//-------------------------------------------------------------------------------------------------

/*!
    Builds the frame of a latitude longitude grid centered on \a gCenter
*/
SGeoFrame CGeoFrame::heightfieldFrame(const CGeoloc& gCenter, const CGeoloc& gSize, int iSampleCount, const Math::CVector3& vFrameOrigin)
{
    SGeoFrame cFrame;

    CGeoloc gDatum(gCenter.Latitude, gCenter.Longitude, 0.0);
    Math::CAxis aAxis = gDatum.getTopocentricAxis();
    Math::CVector3 vCenter = gDatum.toVector3();

    cFrame.Position = vCenter - vFrameOrigin;
    cFrame.Rotation = Math::CMatrix4::fromBasisVectors(aAxis.Right, aAxis.Up, aAxis.Front);

    if (iSampleCount > 1)
    {
        double dWestLon = gCenter.Longitude - gSize.Longitude * 0.5;
        double dEastLon = gCenter.Longitude + gSize.Longitude * 0.5;
        double dSouthLat = gCenter.Latitude - gSize.Latitude * 0.5;
        double dNorthLat = gCenter.Latitude + gSize.Latitude * 0.5;

        double dWidthM = (CGeoloc(gCenter.Latitude, dEastLon, 0.0).toVector3() - CGeoloc(gCenter.Latitude, dWestLon, 0.0).toVector3()).magnitude();
        double dDepthM = (CGeoloc(dNorthLat, gCenter.Longitude, 0.0).toVector3() - CGeoloc(dSouthLat, gCenter.Longitude, 0.0).toVector3()).magnitude();

        cFrame.SpacingXM = float(dWidthM / double(iSampleCount - 1));
        cFrame.SpacingZM = float(dDepthM / double(iSampleCount - 1));
    }

    return cFrame;
}

//-------------------------------------------------------------------------------------------------
