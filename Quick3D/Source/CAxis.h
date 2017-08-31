
#pragma once

// Application
#include "CVector3.h"
#include "CMatrix4.h"

namespace Math
{

struct CAxis
{

public:

    CVector3 Front;
    CVector3 Up;
    CVector3 Right;

    //! Default constructor
    CAxis ()
        : Front(0.0, 0.0, 1.0)
        , Up(0.0, 1.0, 0.0)
        , Right(1.0, 0.0, 0.0)
    {
    }

    //! Constructor with front and up vectors
    CAxis (CVector3 NewFront, CVector3 NewUp)
        : Front(NewFront.normalized())
        , Up(NewUp.normalized())
    {
        Right = Up.cross(Front);
    }

    //! Constructor with front, up and right vectors
    CAxis (CVector3 NewFront, CVector3 NewUp, CVector3 NewRight)
        : Front(NewFront.normalized())
        , Up(NewUp.normalized())
        , Right(NewRight.normalized())
    {
    }

    //! Constructor with euler angles
    CAxis (CVector3 vAngles)
        : Front(0.0, 0.0, 1.0)
        , Up(0.0, 1.0, 0.0)
        , Right(1.0, 0.0, 0.0)
    {
        CMatrix4 mRotationX = CMatrix4::makeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::makeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::makeRotation(CVector3(0.0, 0.0, vAngles.Z));

        Front = mRotationY * ((mRotationX * (mRotationZ * Front)));
        Up = mRotationY * ((mRotationX * (mRotationZ * Up)));
        Right = mRotationY * ((mRotationX * (mRotationZ * Right)));
    }

    //! Copy constructor
    CAxis (const CAxis& Target)
    {
        *this = Target;
    }

    //! Assign operator
    inline CAxis& operator = (const CAxis& Target)
    {
        Front = Target.Front;
        Up = Target.Up;
        Right = Target.Right;

        return *this;
    }

    //! Rotate the axis using euler angles
    inline CAxis rotate(CVector3 vAngles) const
    {
        CAxis aResult(*this);

        CMatrix4 mRotationX = CMatrix4::makeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::makeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::makeRotation(CVector3(0.0, 0.0, vAngles.Z));

        aResult.Front = mRotationY * ((mRotationX * (mRotationZ * aResult.Front)));
        aResult.Up = mRotationY * ((mRotationX * (mRotationZ * aResult.Up)));
        aResult.Right = mRotationY * ((mRotationX * (mRotationZ * aResult.Right)));

        return aResult;
    }

    //! Transfers this axis to another axis frame
    inline CAxis transferTo(CAxis aTarget) const
    {
        CAxis aResult(*this);

        CVector3 vAngles = aTarget.eulerAngles();

        CMatrix4 mRotationX = CMatrix4::makeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::makeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::makeRotation(CVector3(0.0, 0.0, vAngles.Z));

        aResult.Front = mRotationY * ((mRotationX * (mRotationZ * aResult.Front)));
        aResult.Up = mRotationY * ((mRotationX * (mRotationZ * aResult.Up)));
        aResult.Right = mRotationY * ((mRotationX * (mRotationZ * aResult.Right)));

        return aResult;

        /*
            Matrix4 mTransform(Right, Up, Front);

            return Axis(mTransform * aTarget.Right, mTransform * aTarget.Up, mTransform * aTarget.Front);
            */
    }

    //! Transfers this axis from another axis frame
    inline CAxis transferFrom(CAxis aTarget) const
    {
        CAxis aResult(*this);

        CVector3 vAngles = aTarget.eulerAngles();

        CMatrix4 mRotationX = CMatrix4::makeRotation(CVector3(-vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::makeRotation(CVector3(0.0, -vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::makeRotation(CVector3(0.0, 0.0, -vAngles.Z));

        aResult.Front = mRotationZ * ((mRotationX * (mRotationY * aResult.Front)));
        aResult.Up = mRotationZ * ((mRotationX * (mRotationY * aResult.Up)));
        aResult.Right = mRotationZ * ((mRotationX * (mRotationY * aResult.Right)));

        return aResult;
    }

    //! Multiplies this axis with a matrix
    inline CAxis operator * (const CMatrix4& mMatrix) const
    {
        CAxis aResult(*this);

        CVector3 vOrigin = mMatrix * CVector3();

        aResult.Front = (mMatrix * aResult.Front) - vOrigin;
        aResult.Up = (mMatrix * aResult.Up) - vOrigin;
        aResult.Right = (mMatrix * aResult.Right) - vOrigin;

        return aResult;
    }

    //! returns the euler angles of this axis
    inline CVector3 eulerAngles() const
    {
        CVector3 vDiff1 = Front;
        CVector3 vDiff2 = Right;

        double dY = vDiff1.eulerYAngle();

        CMatrix4 mRotationCancelPan = CMatrix4::makeRotation(CVector3(0.0, vDiff1.eulerYAngle() * -1.0, 0.0));

        vDiff1 = mRotationCancelPan * vDiff1;
        vDiff2 = mRotationCancelPan * vDiff2;

        double dX = vDiff1.eulerXAngle();

        CMatrix4 mRotationCancelTilt = CMatrix4::makeRotation(CVector3(vDiff1.eulerXAngle() * -1.0, 0.0, 0.0));

        vDiff1 = mRotationCancelTilt * vDiff1;
        vDiff2 = mRotationCancelTilt * vDiff2;

        double dZ = vDiff2.eulerZAngle();

        dX = Math::Angles::clipAngleRadianPIMinusPI(dX);
        dY = Math::Angles::clipAngleRadianPIMinusPI(dY);
        dZ = Math::Angles::clipAngleRadianPIMinusPI(dZ);

        return CVector3(dX, dY, dZ);
    }
};

}
