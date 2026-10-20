
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

/*!
    Geographic grid frame for height field registration. \br\br
    Engine only header, no physics dependency: the local frame has +X east,
    +Y up and +Z north at the grid center datum. Physics layers convert the
    returned position and rotation into their own body frames.
*/
struct QUICK3D_EXPORT SGeoFrame
{
    Math::CVector3 Position;                ///< Grid center at zero datum in caller frame
    Math::CMatrix4 Rotation;                ///< Local to caller frame rotation
    float SpacingXM = 1.0f;                 ///< Eastward sample spacing in meters
    float SpacingZM = 1.0f;                 ///< Northward sample spacing in meters
};

//-------------------------------------------------------------------------------------------------

/*!
    Geographic frame helper, no physics dependency. \br\br
    Builds the frame of a latitude longitude grid centered on \a gCenter.
    \a gSize holds the grid span in degrees, \a iSampleCount the samples
    per side, \a vFrameOrigin the caller frame origin subtracted from the
    grid center. Spacings use exact ECEF edge lengths, the planar
    approximation drops earth curvature over the grid span.
*/
class QUICK3D_EXPORT CGeoFrame
{

public:

    //! Builds the frame of a latitude longitude grid centered on \a gCenter
    static SGeoFrame heightfieldFrame(const CGeoloc& gCenter, const CGeoloc& gSize, int iSampleCount, const Math::CVector3& vFrameOrigin);
};

//-------------------------------------------------------------------------------------------------
