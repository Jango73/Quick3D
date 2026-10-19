
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCollisionResolver
{
public:

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the inverse mass for \a massKg, or 0.0 when \a movable is false.
    //! Objects with null or negative mass are treated as immovable.
    static double inverseMass(double massKg, bool movable);

    //! Resolves a sphere contact between two bodies, arcade style (no energy creation).
    //! \a vectorFromFirstToSecond is the offset from the first body to the second, in meters.
    //! \a radiusSumM is the sum of both bounding radii, in meters.
    //! Velocities are in meters per second, corrections in meters.
    //! Returns \c true when spheres intersect.
    //! When bodies separate or both are immovable, velocities are left unchanged.
    static bool resolveSphereContact(
            double firstMassKg,
            const Math::CVector3& firstVelocityMs,
            bool firstMovable,
            double secondMassKg,
            const Math::CVector3& secondVelocityMs,
            bool secondMovable,
            const Math::CVector3& vectorFromFirstToSecond,
            double radiusSumM,
            double restitutionNorm,
            double positionCorrectionFactor,
            Math::CVector3& firstVelocityOutMs,
            Math::CVector3& secondVelocityOutMs,
            Math::CVector3& firstCorrectionM,
            Math::CVector3& secondCorrectionM
            );
};
