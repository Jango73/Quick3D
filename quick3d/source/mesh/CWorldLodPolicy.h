#pragma once

#include "quick3d_global.h"
#include "CGeoloc.h"

//!
//! Concentrates the quadtree level-of-detail policy: every chunk asks this
//! class what to do in its state, instead of spreading the rules across the
//! build, collect and garbage passes. \br\br
//! One zone is drawn by exactly one level: a node whose four children are all
//! ready hands over to them and steps aside, otherwise the node draws its own
//! patch and the recursion stops there. Drawing two levels of the same zone
//! in the same layer pass is forbidden by construction, not by an added gate.
//! \br\br
//! A node whose four children are ready may destroy its patch, except on the
//! two coarsest levels which are never destroyed: they are the fallback cover
//! collected when nothing finer is ready. Children that are no longer needed
//! tell their parent, which rebuilds its patch when missing before they go.
//!
class QUICK3D_EXPORT CWorldLodPolicy
{
public:

    //! Children created by one subdivision: the only complete child set
    static constexpr int FullChildCount = 4;

    //! Camera distance in ground diagonals under which a chunk stays needed, keeping nearby chunks behind the camera alive for rotations
    static constexpr double NeededRangeFactor = 2.0;

    //-------------------------------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------------------------------

    //! Draw decision for one chunk in its current state
    enum ELodDrawDecision
    {
        LodDrawSelf,        ///< Children incomplete: draw the own patch, do not descend
        LodHandToChildren,  ///< Four children ready: hand over to them, do not draw self
        LodDrawNothing       ///< Self not ready and children incomplete: draw nothing here
    };

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Draw decision from readiness: four ready children hand over, otherwise the ready self draws
    static ELodDrawDecision decideDraw(bool bSelfReady, int iReadyChildren, int iChildCount);

    //! Tells whether the four children are all ready, the only hand-over case
    static bool allChildrenReady(int iReadyChildren, int iChildCount);

    //! Tells whether the child set is partial, neither empty nor complete: never a valid subdivision
    static bool hasPartialChildren(int iChildCount);

    //! Tells whether \a iLevel is one of the two coarsest levels, never destroyed
    static bool isProtectedLevel(int iLevel, int iMaxLevels);

    //! A node may destroy its patch when the children fully cover it, unless protected
    static bool mayDestroySelf(bool bCovered, int iLevel, int iMaxLevels, bool bNeeded);

    //! Ground diagonal in meters under which a chunk is needed by the camera
    static double neededRange(const CGeoloc& gSize);

    //! Tells whether \a gCamera needs the ground extent: inside or close enough to draw it soon
    static bool isNeeded(const CGeoloc& gCenter, const CGeoloc& gSize, const CGeoloc& gCamera);

    //! Tells whether two ground extents overlap: two collected chunks sharing ground mean double draw
    static bool extentsOverlap(const CGeoloc& gCenterA, const CGeoloc& gSizeA, const CGeoloc& gCenterB, const CGeoloc& gSizeB);
};
