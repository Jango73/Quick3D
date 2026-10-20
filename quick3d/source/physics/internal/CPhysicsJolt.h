
#pragma once

// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

// Application
#include "CVector3.h"
#include "CQuaternion.h"

//-------------------------------------------------------------------------------------------------

/*!
    Engine to Jolt vector and rotation conversions. \br\br
    Internal header: Jolt types appear in the signatures, include it from
    engine source files only, never from headers consumed by downstream
    applications. This folder is excluded from the Quick3D.pri header glob
    on purpose. \br\br
    Single precision build aliases RVec3 to Vec3, so one helper pair covers
    both directions and positions.
*/
class CPhysicsJolt
{

public:

    //! Converts an engine vector to a Jolt vector
    static inline JPH::Vec3 toJolt(const Math::CVector3& vVector)
    {
        return JPH::Vec3(float(vVector.X), float(vVector.Y), float(vVector.Z));
    }

    //! Converts a Jolt vector to an engine vector
    static inline Math::CVector3 fromJolt(const JPH::Vec3& vVector)
    {
        return Math::CVector3(double(vVector.GetX()), double(vVector.GetY()), double(vVector.GetZ()));
    }

    //! Converts an engine quaternion to a Jolt quaternion, component by component
    static inline JPH::Quat toJoltRotation(const Math::CQuaternion& cQuaternion)
    {
        return JPH::Quat(float(cQuaternion.X), float(cQuaternion.Y), float(cQuaternion.Z), float(cQuaternion.W));
    }

    //! Converts a Jolt quaternion to an engine quaternion, component by component
    static inline Math::CQuaternion fromJoltRotation(const JPH::Quat& cQuaternion)
    {
        return Math::CQuaternion(double(cQuaternion.GetX()), double(cQuaternion.GetY()), double(cQuaternion.GetZ()), double(cQuaternion.GetW()));
    }
};

//-------------------------------------------------------------------------------------------------
