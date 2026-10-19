
// Application
#include "CCollisionResolver.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Returns the inverse mass for \a massKg, or 0.0 when \a movable is false. \br\br
    Objects with null or negative mass are treated as immovable.
*/
double CCollisionResolver::inverseMass(double massKg, bool movable)
{
    if (movable == false)
    {
        return 0.0;
    }

    if (massKg <= 0.0)
    {
        return 0.0;
    }

    return 1.0 / massKg;
}

/***************************************************************************/

/*!
    Resolves a sphere contact between two bodies, arcade style (no energy creation). \br\br
    \a vectorFromFirstToSecond is the offset from the first body to the second, in meters. \br
    \a radiusSumM is the sum of both bounding radii, in meters. \br
    Velocities are in meters per second, corrections in meters. \br\br
    Position correction pushes bodies apart proportionally to their inverse mass. \br
    Velocity change applies only while bodies approach, with a \a restitutionNorm
    bounce factor (0.0 keeps the normal approach speed at zero). \br\br
    Returns \c true when spheres intersect. \br
    When bodies separate or both are immovable, velocities are left unchanged.
*/
bool CCollisionResolver::resolveSphereContact(
        double firstMassKg,
        const CVector3& firstVelocityMs,
        bool firstMovable,
        double secondMassKg,
        const CVector3& secondVelocityMs,
        bool secondMovable,
        const CVector3& vectorFromFirstToSecond,
        double radiusSumM,
        double restitutionNorm,
        double positionCorrectionFactor,
        CVector3& firstVelocityOutMs,
        CVector3& secondVelocityOutMs,
        CVector3& firstCorrectionM,
        CVector3& secondCorrectionM
        )
{
    firstVelocityOutMs = firstVelocityMs;
    secondVelocityOutMs = secondVelocityMs;
    firstCorrectionM = CVector3();
    secondCorrectionM = CVector3();

    double dDistanceM = vectorFromFirstToSecond.magnitude();

    if (dDistanceM >= radiusSumM)
    {
        return false;
    }

    CVector3 vNormal;

    if (dDistanceM > 0.000001)
    {
        vNormal = vectorFromFirstToSecond / dDistanceM;
    }
    else
    {
        vNormal = CVector3(1.0, 0.0, 0.0);
        dDistanceM = 0.0;
    }

    double dInverseFirst = inverseMass(firstMassKg, firstMovable);
    double dInverseSecond = inverseMass(secondMassKg, secondMovable);
    double dInverseTotal = dInverseFirst + dInverseSecond;

    if (dInverseTotal <= 0.0)
    {
        return true;
    }

    double dClampedFactor = positionCorrectionFactor;

    if (dClampedFactor < 0.0)
    {
        dClampedFactor = 0.0;
    }

    if (dClampedFactor > 1.0)
    {
        dClampedFactor = 1.0;
    }

    double dPenetrationM = radiusSumM - dDistanceM;

    firstCorrectionM = vNormal * (-dPenetrationM * (dInverseFirst / dInverseTotal) * dClampedFactor);
    secondCorrectionM = vNormal * (dPenetrationM * (dInverseSecond / dInverseTotal) * dClampedFactor);

    CVector3 vRelativeVelocityMs = firstVelocityMs - secondVelocityMs;
    double dApproachSpeedMs = vRelativeVelocityMs.dot(vNormal);

    if (dApproachSpeedMs <= 0.0)
    {
        return true;
    }

    double dClampedRestitution = restitutionNorm;

    if (dClampedRestitution < 0.0)
    {
        dClampedRestitution = 0.0;
    }

    if (dClampedRestitution > 1.0)
    {
        dClampedRestitution = 1.0;
    }

    double dImpulse = (1.0 + dClampedRestitution) * dApproachSpeedMs / dInverseTotal;

    firstVelocityOutMs = firstVelocityMs - vNormal * (dImpulse * dInverseFirst);
    secondVelocityOutMs = secondVelocityMs + vNormal * (dImpulse * dInverseSecond);

    return true;
}
