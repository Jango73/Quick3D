#include "CWorldLodPolicy.h"

/***************************************************************************/
CWorldLodPolicy::ELodDrawDecision CWorldLodPolicy::decideDraw(bool bSelfReady, int iReadyChildren, int iChildCount)
{
    if (allChildrenReady(iReadyChildren, iChildCount))
    {
        return LodHandToChildren;
    }

    if (bSelfReady)
    {
        return LodDrawSelf;
    }

    return LodDrawNothing;
}

/***************************************************************************/
bool CWorldLodPolicy::allChildrenReady(int iReadyChildren, int iChildCount)
{
    return iChildCount == 4 && iReadyChildren == 4;
}

/***************************************************************************/
bool CWorldLodPolicy::isProtectedLevel(int iLevel, int iMaxLevels)
{
    return iLevel >= iMaxLevels - 1;
}

/***************************************************************************/
bool CWorldLodPolicy::mayDestroySelf(bool bCovered, int iLevel, int iMaxLevels, bool bNeeded)
{
    return bCovered && isProtectedLevel(iLevel, iMaxLevels) == false && bNeeded == false;
}

//! Camera distance in ground diagonals under which a chunk stays needed,
//! keeping nearby chunks behind the camera alive for rotations
static const double NeededRangeFactor = 2.0;

/***************************************************************************/
double CWorldLodPolicy::neededRange(const CGeoloc& gSize)
{
    CGeoloc gCornerA(gSize.Latitude * -0.5, gSize.Longitude * -0.5, 0.0);
    CGeoloc gCornerB(gSize.Latitude * 0.5, gSize.Longitude * 0.5, 0.0);

    return gCornerA.distanceTo(gCornerB) * NeededRangeFactor;
}

/***************************************************************************/
bool CWorldLodPolicy::isNeeded(const CGeoloc& gCenter, const CGeoloc& gSize, const CGeoloc& gCamera)
{
    return (gCenter.toVector3() - gCamera.toVector3()).magnitude() < neededRange(gSize);
}

/***************************************************************************/
bool CWorldLodPolicy::extentsOverlap(const CGeoloc& gCenterA, const CGeoloc& gSizeA, const CGeoloc& gCenterB, const CGeoloc& gSizeB)
{
    if (qAbs(gCenterA.Latitude - gCenterB.Latitude) >= (gSizeA.Latitude + gSizeB.Latitude) * 0.5) return false;
    if (qAbs(gCenterA.Longitude - gCenterB.Longitude) >= (gSizeA.Longitude + gSizeB.Longitude) * 0.5) return false;

    return true;
}
