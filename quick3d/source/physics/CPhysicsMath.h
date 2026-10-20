
#pragma once

// Application
#include "CMatrix4.h"
#include "CQuaternion.h"

//-------------------------------------------------------------------------------------------------

/*!
    Engine rotation conversion shared with the physics layer. \br\br
    Downstream safe header: no Jolt type appears here. Jolt vector and
    quaternion conversions live in source/physics/internal/CPhysicsJolt.h,
    geographic grid frames in source/math/CGeoFrame.h. \br\br
    Rotation matrices use the engine Data[row][col] layout, matching
    CMatrix4::fromQuaternion, which toQuaternion() inverts exactly.
*/
class CPhysicsMath
{

public:

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
};

//-------------------------------------------------------------------------------------------------
