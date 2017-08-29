
#pragma once

// Std
#include "string.h"
#include "math.h"

// Qt
#include <QMatrix4x4>

// Application
#include "CVector3.h"
#include "CRay3.h"
#include "CQuaternion.h"

/*-------------------------------------------------------------------------------------------------
    Axis conventions

    The conventions used throughout the vector/matrix classes in this library are those of the painter.

    X axis is positive right (width)
    Y axis is positive up (height)
    Z axis is positive front (depth)

    A positive rotation around the X axis (pitch) makes the Z axis go down
    A positive rotation around the Y axis (yaw) makes the Z axis go right
    A positive rotation around the Z axis (roll) makes the X axis go up

    Y
    ^    Z
    |   A
    |  /
    | /
    |/
    --------> X

    Rotation orders

    The axis with the most weight is the Y axis because it represents the yaw or heading (most important angular information in world positioning).
    The axis with the least weight is the Z axis.

    So when applying a rotation from local to world frames, the order of angle application must be : Z, X, Y.
    And from world to local, it will be : -Y, -X, -Z.

-------------------------------------------------------------------------------------------------*/

namespace Math
{

struct CMatrix4
{

public:

    bool        m_bIsIdentity;
    double      Data [4][4];

    //! Default constructor
    inline CMatrix4()
        : m_bIsIdentity(false)
    {
        makeIdentity();
    }

    inline CMatrix4(CVector3 a, CVector3 b, CVector3 c)
        : m_bIsIdentity(false)
    {
        Data[0][0] = a.X;
        Data[0][1] = a.Y;
        Data[0][2] = a.Z;
        Data[0][3] = 0.0;

        Data[1][0] = b.X;
        Data[1][1] = b.Y;
        Data[1][2] = b.Z;
        Data[1][3] = 0.0;

        Data[2][0] = c.X;
        Data[2][1] = c.Y;
        Data[2][2] = c.Z;
        Data[2][3] = 0.0;

        Data[3][0] = 0.0;
        Data[3][1] = 0.0;
        Data[3][2] = 0.0;
        Data[3][3] = 1.0;
    }

    //! Makes an identity matrix
    inline void makeIdentity()
    {
        m_bIsIdentity = true;

        Data[0][0] = 1.0;
        Data[0][1] = 0.0;
        Data[0][2] = 0.0;
        Data[0][3] = 0.0;

        Data[1][0] = 0.0;
        Data[1][1] = 1.0;
        Data[1][2] = 0.0;
        Data[1][3] = 0.0;

        Data[2][0] = 0.0;
        Data[2][1] = 0.0;
        Data[2][2] = 1.0;
        Data[2][3] = 0.0;

        Data[3][0] = 0.0;
        Data[3][1] = 0.0;
        Data[3][2] = 0.0;
        Data[3][3] = 1.0;
    }

    //! Returns \c true if this matrix is identity
    inline bool isIdentity() const { return m_bIsIdentity; }

    //! Creates a translation matrix
    static inline CMatrix4 makeTranslation(const CVector3& Translation)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        Result.Data[3][0] = Translation.X;
        Result.Data[3][1] = Translation.Y;
        Result.Data[3][2] = Translation.Z;

        return Result;
    }

    //! Crée une matrice de rotation selon angles euler
    static inline CMatrix4 makeRotation(const CVector3& Rotation)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        // Rotate about the Z-Axis
        if (Rotation.Z != 0.0)
        {
            CMatrix4 H_Rot_Z;

            H_Rot_Z.Data[0][0] = H_Rot_Z.Data[1][1] = cos(Rotation.Z);
            H_Rot_Z.Data[0][1] = sin(Rotation.Z);
            H_Rot_Z.Data[1][0] = -H_Rot_Z.Data[0][1];

            Result = Result * H_Rot_Z;
        }

        // Rotate about the X-Axis
        if (Rotation.X != 0.0)
        {
            CMatrix4 H_Rot_X;

            H_Rot_X.Data[1][1] = H_Rot_X.Data[2][2] = cos(Rotation.X);
            H_Rot_X.Data[1][2] = sin(Rotation.X);
            H_Rot_X.Data[2][1] = -H_Rot_X.Data[1][2];

            Result = Result * H_Rot_X;
        }

        // Rotate about the Y-Axis
        if (Rotation.Y != 0.0)
        {
            CMatrix4 H_Rot_Y;

            H_Rot_Y.Data[0][0] = H_Rot_Y.Data[2][2] = cos(Rotation.Y);
            H_Rot_Y.Data[2][0] = sin(Rotation.Y);
            H_Rot_Y.Data[0][2] = -H_Rot_Y.Data[2][0];

            Result = Result * H_Rot_Y;
        }

        return Result;
    }

    //! Crée une matrice de rotation selon angles euler
    static inline CMatrix4 makeInverseRotation(const CVector3& Rotation)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        // Rotate about the Y-Axis
        if (Rotation.Y != 0.0)
        {
            CMatrix4 H_Rot_Y;

            H_Rot_Y.Data[0][0] = H_Rot_Y.Data[2][2] = cos(Rotation.Y);
            H_Rot_Y.Data[2][0] = sin(Rotation.Y);
            H_Rot_Y.Data[0][2] = -H_Rot_Y.Data[2][0];

            Result = Result * H_Rot_Y;
        }

        // Rotate about the X-Axis
        if (Rotation.X != 0.0)
        {
            CMatrix4 H_Rot_X;

            H_Rot_X.Data[1][1] = H_Rot_X.Data[2][2] = cos(Rotation.X);
            H_Rot_X.Data[1][2] = sin(Rotation.X);
            H_Rot_X.Data[2][1] = -H_Rot_X.Data[1][2];

            Result = Result * H_Rot_X;
        }

        // Rotate about the Z-Axis
        if (Rotation.Z != 0.0)
        {
            CMatrix4 H_Rot_Z;

            H_Rot_Z.Data[0][0] = H_Rot_Z.Data[1][1] = cos(Rotation.Z);
            H_Rot_Z.Data[0][1] = sin(Rotation.Z);
            H_Rot_Z.Data[1][0] = -H_Rot_Z.Data[0][1];

            Result = Result * H_Rot_Z;
        }

        return Result;
    }

    //! Crée une matrice de mise à l'échelle
    static inline CMatrix4 makeScale(const CVector3& Scale)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        Result.Data[0][0] = Scale.X;
        Result.Data[1][1] = Scale.Y;
        Result.Data[2][2] = Scale.Z;
        Result.Data[3][3] = 1.0;

        return Result;
    }

    //!
    static inline CMatrix4 makePerspective(double dFov, double dAspectRatio, double dNear, double dFar)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        double xScale = 1.0 / (tan(dFov * 0.5));
        double yScale = xScale / dAspectRatio;

        Result.Data[0][0] = xScale;
        Result.Data[1][1] = yScale;
        Result.Data[2][2] = -(dFar + dNear) / (dFar - dNear);
        Result.Data[2][3] = -1.0;
        Result.Data[3][2] = -2 * dNear * dFar / (dFar - dNear);
        Result.Data[3][3] = 0.0;

        return Result;
    }

    inline CMatrix4& operator = (const CMatrix4& target)
    {
        m_bIsIdentity = target.m_bIsIdentity;
        memcpy(Data, target.Data, sizeof(Data));

        return *this;
    }

    //! Multiplies two matrices
    inline CMatrix4 operator * (const CMatrix4& m2)
    {
        CMatrix4 m;

        m.m_bIsIdentity = false;

        m.Data[0][0] = Data[0][0] * m2.Data[0][0] + Data[0][1] * m2.Data[1][0] + Data[0][2] * m2.Data[2][0] + Data[0][3] * m2.Data[3][0];
        m.Data[0][1] = Data[0][0] * m2.Data[0][1] + Data[0][1] * m2.Data[1][1] + Data[0][2] * m2.Data[2][1] + Data[0][3] * m2.Data[3][1];
        m.Data[0][2] = Data[0][0] * m2.Data[0][2] + Data[0][1] * m2.Data[1][2] + Data[0][2] * m2.Data[2][2] + Data[0][3] * m2.Data[3][2];
        m.Data[0][3] = Data[0][0] * m2.Data[0][3] + Data[0][1] * m2.Data[1][3] + Data[0][2] * m2.Data[2][3] + Data[0][3] * m2.Data[3][3];
        m.Data[1][0] = Data[1][0] * m2.Data[0][0] + Data[1][1] * m2.Data[1][0] + Data[1][2] * m2.Data[2][0] + Data[1][3] * m2.Data[3][0];
        m.Data[1][1] = Data[1][0] * m2.Data[0][1] + Data[1][1] * m2.Data[1][1] + Data[1][2] * m2.Data[2][1] + Data[1][3] * m2.Data[3][1];
        m.Data[1][2] = Data[1][0] * m2.Data[0][2] + Data[1][1] * m2.Data[1][2] + Data[1][2] * m2.Data[2][2] + Data[1][3] * m2.Data[3][2];
        m.Data[1][3] = Data[1][0] * m2.Data[0][3] + Data[1][1] * m2.Data[1][3] + Data[1][2] * m2.Data[2][3] + Data[1][3] * m2.Data[3][3];
        m.Data[2][0] = Data[2][0] * m2.Data[0][0] + Data[2][1] * m2.Data[1][0] + Data[2][2] * m2.Data[2][0] + Data[2][3] * m2.Data[3][0];
        m.Data[2][1] = Data[2][0] * m2.Data[0][1] + Data[2][1] * m2.Data[1][1] + Data[2][2] * m2.Data[2][1] + Data[2][3] * m2.Data[3][1];
        m.Data[2][2] = Data[2][0] * m2.Data[0][2] + Data[2][1] * m2.Data[1][2] + Data[2][2] * m2.Data[2][2] + Data[2][3] * m2.Data[3][2];
        m.Data[2][3] = Data[2][0] * m2.Data[0][3] + Data[2][1] * m2.Data[1][3] + Data[2][2] * m2.Data[2][3] + Data[2][3] * m2.Data[3][3];
        m.Data[3][0] = Data[3][0] * m2.Data[0][0] + Data[3][1] * m2.Data[1][0] + Data[3][2] * m2.Data[2][0] + Data[3][3] * m2.Data[3][0];
        m.Data[3][1] = Data[3][0] * m2.Data[0][1] + Data[3][1] * m2.Data[1][1] + Data[3][2] * m2.Data[2][1] + Data[3][3] * m2.Data[3][1];
        m.Data[3][2] = Data[3][0] * m2.Data[0][2] + Data[3][1] * m2.Data[1][2] + Data[3][2] * m2.Data[2][2] + Data[3][3] * m2.Data[3][2];
        m.Data[3][3] = Data[3][0] * m2.Data[0][3] + Data[3][1] * m2.Data[1][3] + Data[3][2] * m2.Data[2][3] + Data[3][3] * m2.Data[3][3];

        return m;
    }

    //! Multiplies a vector and an affine transform matrix
    inline CVector3 operator * (const CVector3& a) const
    {
        CVector3 c;

        c.X = Data[0][0] * a.X + Data[1][0] * a.Y + Data[2][0] * a.Z + Data[3][0];
        c.Y = Data[0][1] * a.X + Data[1][1] * a.Y + Data[2][1] * a.Z + Data[3][1];
        c.Z = Data[0][2] * a.X + Data[1][2] * a.Y + Data[2][2] * a.Z + Data[3][2];

        return c;
    }

    //! Multiplies a vector and a projection matrix
    inline CVector3 project (const CVector3& a) const
    {
        CVector3 c;

        c.X			= Data[0][0] * a.X + Data[1][0] * a.Y + Data[2][0] * a.Z + Data[3][0];
        c.Y			= Data[0][1] * a.X + Data[1][1] * a.Y + Data[2][1] * a.Z + Data[3][1];
        c.Z			= Data[0][2] * a.X + Data[1][2] * a.Y + Data[2][2] * a.Z + Data[3][2];
        double w	= Data[0][3] * a.X + Data[1][3] * a.Y + Data[2][3] * a.Z + Data[3][3];

        if (w != 1.0 && w != 0.0) c = c / w;

        return c;
    }

    //! Multiplies a ray and a projection matrix
    inline CRay3 operator * (const CRay3& a) const
    {
        CRay3 aRay;

        aRay.vOrigin = *this * a.vOrigin;

        aRay.vNormal.X	= Data[0][0] * a.vNormal.X + Data[1][0] * a.vNormal.Y + Data[2][0] * a.vNormal.Z;
        aRay.vNormal.Y	= Data[0][1] * a.vNormal.X + Data[1][1] * a.vNormal.Y + Data[2][1] * a.vNormal.Z;
        aRay.vNormal.Z	= Data[0][2] * a.vNormal.X + Data[1][2] * a.vNormal.Y + Data[2][2] * a.vNormal.Z;

        return aRay;
    }

    inline CMatrix4 inverse()
    {
        CMatrix4 m;

        m.m_bIsIdentity = false;

        double s0 = Data[0][0] * Data[1][1] - Data[1][0] * Data[0][1];
        double s1 = Data[0][0] * Data[1][2] - Data[1][0] * Data[0][2];
        double s2 = Data[0][0] * Data[1][3] - Data[1][0] * Data[0][3];
        double s3 = Data[0][1] * Data[1][2] - Data[1][1] * Data[0][2];
        double s4 = Data[0][1] * Data[1][3] - Data[1][1] * Data[0][3];
        double s5 = Data[0][2] * Data[1][3] - Data[1][2] * Data[0][3];

        double c5 = Data[2][2] * Data[3][3] - Data[3][2] * Data[2][3];
        double c4 = Data[2][1] * Data[3][3] - Data[3][1] * Data[2][3];
        double c3 = Data[2][1] * Data[3][2] - Data[3][1] * Data[2][2];
        double c2 = Data[2][0] * Data[3][3] - Data[3][0] * Data[2][3];
        double c1 = Data[2][0] * Data[3][2] - Data[3][0] * Data[2][2];
        double c0 = Data[2][0] * Data[3][1] - Data[3][0] * Data[2][1];

        // Should check for 0 determinant

        double invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

        m.Data[0][0] = ( Data[1][1] * c5 - Data[1][2] * c4 + Data[1][3] * c3) * invdet;
        m.Data[0][1] = (-Data[0][1] * c5 + Data[0][2] * c4 - Data[0][3] * c3) * invdet;
        m.Data[0][2] = ( Data[3][1] * s5 - Data[3][2] * s4 + Data[3][3] * s3) * invdet;
        m.Data[0][3] = (-Data[2][1] * s5 + Data[2][2] * s4 - Data[2][3] * s3) * invdet;

        m.Data[1][0] = (-Data[1][0] * c5 + Data[1][2] * c2 - Data[1][3] * c1) * invdet;
        m.Data[1][1] = ( Data[0][0] * c5 - Data[0][2] * c2 + Data[0][3] * c1) * invdet;
        m.Data[1][2] = (-Data[3][0] * s5 + Data[3][2] * s2 - Data[3][3] * s1) * invdet;
        m.Data[1][3] = ( Data[2][0] * s5 - Data[2][2] * s2 + Data[2][3] * s1) * invdet;

        m.Data[2][0] = ( Data[1][0] * c4 - Data[1][1] * c2 + Data[1][3] * c0) * invdet;
        m.Data[2][1] = (-Data[0][0] * c4 + Data[0][1] * c2 - Data[0][3] * c0) * invdet;
        m.Data[2][2] = ( Data[3][0] * s4 - Data[3][1] * s2 + Data[3][3] * s0) * invdet;
        m.Data[2][3] = (-Data[2][0] * s4 + Data[2][1] * s2 - Data[2][3] * s0) * invdet;

        m.Data[3][0] = (-Data[1][0] * c3 + Data[1][1] * c1 - Data[1][2] * c0) * invdet;
        m.Data[3][1] = ( Data[0][0] * c3 - Data[0][1] * c1 + Data[0][2] * c0) * invdet;
        m.Data[3][2] = (-Data[3][0] * s3 + Data[3][1] * s1 - Data[3][2] * s0) * invdet;
        m.Data[3][3] = ( Data[2][0] * s3 - Data[2][1] * s1 + Data[2][2] * s0) * invdet;

        return m;
    }

    //! Crée une matrice de rotation d'après un quaternion
    static inline CMatrix4 fromQuaternion(const CQuaternion& Quat)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        double sqw = Quat.W * Quat.W;
        double sqx = Quat.X * Quat.X;
        double sqy = Quat.Y * Quat.Y;
        double sqz = Quat.Z * Quat.Z;

        // invs (inverse square length) is only required if quaternion is not already normalised
        double invs = 1 / (sqx + sqy + sqz + sqw);
        Result.Data[0][0] = (sqx - sqy - sqz + sqw) * invs; // since sqw + sqx + sqy + sqz =1/invs*invs
        Result.Data[1][1] = (-sqx + sqy - sqz + sqw) * invs;
        Result.Data[2][2] = (-sqx - sqy + sqz + sqw) * invs;

        double tmp1 = Quat.X * Quat.Y;
        double tmp2 = Quat.Z * Quat.W;
        Result.Data[1][0] = 2.0 * (tmp1 + tmp2) * invs;
        Result.Data[0][1] = 2.0 * (tmp1 - tmp2) * invs;

        tmp1 = Quat.X * Quat.Z;
        tmp2 = Quat.Y * Quat.W;
        Result.Data[2][0] = 2.0 * (tmp1 - tmp2) * invs;
        Result.Data[0][2] = 2.0 * (tmp1 + tmp2) * invs;

        tmp1 = Quat.Y * Quat.Z;
        tmp2 = Quat.X * Quat.W;
        Result.Data[2][1] = 2.0 * (tmp1 + tmp2) * invs;
        Result.Data[1][2] = 2.0 * (tmp1 - tmp2) * invs;

        return Result;
    }

    static inline CMatrix4 fromOpenGLMatrix(const float* mOpenGLMat)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        Result.Data[0][0] = (double) mOpenGLMat[ 0];
        Result.Data[0][1] = (double) mOpenGLMat[ 4];
        Result.Data[0][2] = (double) mOpenGLMat[ 8];
        Result.Data[0][3] = (double) mOpenGLMat[12];

        Result.Data[1][0] = (double) mOpenGLMat[ 1];
        Result.Data[1][1] = (double) mOpenGLMat[ 5];
        Result.Data[1][2] = (double) mOpenGLMat[ 9];
        Result.Data[1][3] = (double) mOpenGLMat[13];

        Result.Data[2][0] = (double) mOpenGLMat[ 2];
        Result.Data[2][1] = (double) mOpenGLMat[ 6];
        Result.Data[2][2] = (double) mOpenGLMat[10];
        Result.Data[2][3] = (double) mOpenGLMat[14];

        Result.Data[3][0] = (double) mOpenGLMat[ 3];
        Result.Data[3][1] = (double) mOpenGLMat[ 7];
        Result.Data[3][2] = (double) mOpenGLMat[11];
        Result.Data[3][3] = (double) mOpenGLMat[15];

        return Result;
    }

    inline void toOpenGLMatrix(float* mOpenGLMat)
    {
        mOpenGLMat[ 0] = (float) Data[0][0];
        mOpenGLMat[ 4] = (float) Data[0][1];
        mOpenGLMat[ 8] = (float) Data[0][2];
        mOpenGLMat[12] = (float) Data[0][3];

        mOpenGLMat[ 1] = (float) Data[1][0];
        mOpenGLMat[ 5] = (float) Data[1][1];
        mOpenGLMat[ 9] = (float) Data[1][2];
        mOpenGLMat[13] = (float) Data[1][3];

        mOpenGLMat[ 2] = (float) Data[2][0];
        mOpenGLMat[ 6] = (float) Data[2][1];
        mOpenGLMat[10] = (float) Data[2][2];
        mOpenGLMat[14] = (float) Data[2][3];

        mOpenGLMat[ 3] = (float) Data[3][0];
        mOpenGLMat[ 7] = (float) Data[3][1];
        mOpenGLMat[11] = (float) Data[3][2];
        mOpenGLMat[15] = (float) Data[3][3];
    }

    static inline CMatrix4 fromQtMatrix(const QMatrix4x4& mQtMat)
    {
        CMatrix4 Result;

        Result.m_bIsIdentity = false;

        Result.Data[0][0] = (double) mQtMat.data()[ 0];
        Result.Data[0][1] = (double) mQtMat.data()[ 4];
        Result.Data[0][2] = (double) mQtMat.data()[ 8];
        Result.Data[0][3] = (double) mQtMat.data()[12];

        Result.Data[1][0] = (double) mQtMat.data()[ 1];
        Result.Data[1][1] = (double) mQtMat.data()[ 5];
        Result.Data[1][2] = (double) mQtMat.data()[ 9];
        Result.Data[1][3] = (double) mQtMat.data()[13];

        Result.Data[2][0] = (double) mQtMat.data()[ 2];
        Result.Data[2][1] = (double) mQtMat.data()[ 6];
        Result.Data[2][2] = (double) mQtMat.data()[10];
        Result.Data[2][3] = (double) mQtMat.data()[14];

        Result.Data[3][0] = (double) mQtMat.data()[ 3];
        Result.Data[3][1] = (double) mQtMat.data()[ 7];
        Result.Data[3][2] = (double) mQtMat.data()[11];
        Result.Data[3][3] = (double) mQtMat.data()[15];

        return Result;
    }

    //! Transforms this matrix into a string
    inline QString toString()
    {
        QString sReturnValue;

        for (int iRow = 0; iRow < 4; iRow++)
        {
            sReturnValue += QString("[%1, %2, %3, %4]")
                    .arg(Data[iRow][0])
                    .arg(Data[iRow][1])
                    .arg(Data[iRow][2])
                    .arg(Data[iRow][3]);
        }

        return sReturnValue;
    }
};

//! Returns the euler angles of a vector (supposing it is a direction vector)
inline CVector3 eulerAngles(CVector3 value)
{
    CVector3 vReturnValue;

    vReturnValue.Y = value.eulerYAngle();
    value = CMatrix4::makeRotation(CVector3(0.0, -vReturnValue.Y, 0.0)) * value;
    vReturnValue.X = value.eulerXAngle();

    return vReturnValue;
}
}
