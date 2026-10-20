
#pragma once

// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

// Application
#include "CVector3.h"
#include "CMatrix4.h"
#include "CQuaternion.h"
#include "CGeoloc.h"
#include "CAxis.h"

//-------------------------------------------------------------------------------------------------

/*!
    Engine to Jolt vector and rotation conversions. \br\br
    Internal header: Jolt types appear in the signatures, include it from
    engine source files only, never from headers consumed by downstream
    applications. \br\br
    Single precision build aliases RVec3 to Vec3, so one helper pair covers
    both directions and positions. \br\br
    Rotation matrices use the engine Data[row][col] layout, matching
    CMatrix4::fromQuaternion, which toQuaternion() inverts exactly.
*/
class CPhysicsMath
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

    //! Converts a rotation matrix to a quaternion (Shepperd method). \br\br
    //! Inverts CMatrix4::fromQuaternion exactly for unit quaternions.
    static inline Math::CQuaternion toQuaternion(const Math::CMatrix4& mRotation)
    {
        // Note: the engine operator * applies the transposed matrix, Shepperd
        // runs on the transposed view so the result matches Jolt quaternions
        double dData[3][3];

        for (int iRow = 0; iRow < 3; iRow++)
        {
            for (int iCol = 0; iCol < 3; iCol++)
            {
                dData[iRow][iCol] = mRotation.Data[iCol][iRow];
            }
        }

        double dTrace = dData[0][0] + dData[1][1] + dData[2][2];
        Math::CQuaternion cQuaternion;

        if (dTrace > 0.0)
        {
            double dRoot = sqrt(dTrace + 1.0);
            cQuaternion.W = 0.5 * dRoot;
            dRoot = 0.5 / dRoot;
            cQuaternion.X = (dData[2][1] - dData[1][2]) * dRoot;
            cQuaternion.Y = (dData[0][2] - dData[2][0]) * dRoot;
            cQuaternion.Z = (dData[1][0] - dData[0][1]) * dRoot;
        }
        else
        {
            int iLargest = 0;

            if (dData[1][1] > dData[0][0])
            {
                iLargest = 1;
            }

            if (dData[2][2] > dData[iLargest][iLargest])
            {
                iLargest = 2;
            }

            if (iLargest == 0)
            {
                double dRoot = sqrt(dData[0][0] - dData[1][1] - dData[2][2] + 1.0);
                cQuaternion.X = 0.5 * dRoot;
                dRoot = 0.5 / dRoot;
                cQuaternion.Y = (dData[0][1] + dData[1][0]) * dRoot;
                cQuaternion.Z = (dData[0][2] + dData[2][0]) * dRoot;
                cQuaternion.W = (dData[2][1] - dData[1][2]) * dRoot;
            }
            else if (iLargest == 1)
            {
                double dRoot = sqrt(dData[1][1] - dData[0][0] - dData[2][2] + 1.0);
                cQuaternion.Y = 0.5 * dRoot;
                dRoot = 0.5 / dRoot;
                cQuaternion.X = (dData[0][1] + dData[1][0]) * dRoot;
                cQuaternion.Z = (dData[1][2] + dData[2][1]) * dRoot;
                cQuaternion.W = (dData[0][2] - dData[2][0]) * dRoot;
            }
            else
            {
                double dRoot = sqrt(dData[2][2] - dData[0][0] - dData[1][1] + 1.0);
                cQuaternion.Z = 0.5 * dRoot;
                dRoot = 0.5 / dRoot;
                cQuaternion.X = (dData[0][2] + dData[2][0]) * dRoot;
                cQuaternion.Y = (dData[1][2] + dData[2][1]) * dRoot;
                cQuaternion.W = (dData[1][0] - dData[0][1]) * dRoot;
            }
        }

        return cQuaternion;
    }

    //! Height field frame parameters for CPhysicsWorld::addHeightfield. \br\br
    //! The local frame has +X east, +Y up and +Z north at \a gCenter.
    struct SHeightfieldFrame
    {
        Math::CVector3 Position;                ///< Grid center at zero datum in caller frame
        Math::CMatrix4 Rotation;                ///< Local to caller frame rotation
        float SpacingXM = 1.0f;                 ///< Eastward sample spacing in meters
        float SpacingZM = 1.0f;                 ///< Northward sample spacing in meters
    };

    //! Builds the frame of a latitude longitude grid centered on \a gCenter. \br\br
    //! \a gSize holds the grid span in degrees, \a iSampleCount the samples
    //! per side, \a vFrameOrigin the caller frame origin subtracted from the
    //! grid center. Spacings use exact ECEF edge lengths, the planar
    //! approximation drops earth curvature over the grid span.
    static inline SHeightfieldFrame heightfieldFrame(const CGeoloc& gCenter, const CGeoloc& gSize, int iSampleCount, const Math::CVector3& vFrameOrigin)
    {
        SHeightfieldFrame cFrame;

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
};

//-------------------------------------------------------------------------------------------------
