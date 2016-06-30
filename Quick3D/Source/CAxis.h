
#ifndef __AXIS_H__
#define __AXIS_H__

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

    //! Constructeur par défaut
    CAxis ()
        : Front(0.0, 0.0, 1.0)
        , Up(0.0, 1.0, 0.0)
        , Right(1.0, 0.0, 0.0)
    {
    }

    //! Constructeur avec composants
    CAxis (CVector3 NewFront, CVector3 NewUp)
        : Front(NewFront)
        , Up(NewUp)
    {
        Right = Up.CrossProduct(Front);
    }

    //! Constructeur avec composants
    CAxis (CVector3 NewFront, CVector3 NewUp, CVector3 NewRight)
        : Front(NewFront)
        , Up(NewUp)
        , Right(NewRight)
    {
    }

    //! Constructeur avec angles euleur
    CAxis (CVector3 vAngles)
        : Front(0.0, 0.0, 1.0)
        , Up(0.0, 1.0, 0.0)
        , Right(1.0, 0.0, 0.0)
    {
        CMatrix4 mRotationX = CMatrix4::MakeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::MakeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::MakeRotation(CVector3(0.0, 0.0, vAngles.Z));

        Front = mRotationY * ((mRotationX * (mRotationZ * Front)));
        Up = mRotationY * ((mRotationX * (mRotationZ * Up)));
        Right = mRotationY * ((mRotationX * (mRotationZ * Right)));
    }

    //! Constructeur de copie
    CAxis (const CAxis& Target)
    {
        *this = Target;
    }

    //! Opérateur de copie
    inline CAxis& operator = (const CAxis& Target)
    {
        Front = Target.Front;
        Up = Target.Up;
        Right = Target.Right;

        return *this;
    }

    //! Transfer d'un axe dans le repère d'un autre axe
    inline CAxis rotate(CVector3 vAngles) const
    {
        CAxis aResult(*this);

        CMatrix4 mRotationX = CMatrix4::MakeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::MakeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::MakeRotation(CVector3(0.0, 0.0, vAngles.Z));

        aResult.Front = mRotationY * ((mRotationX * (mRotationZ * aResult.Front)));
        aResult.Up = mRotationY * ((mRotationX * (mRotationZ * aResult.Up)));
        aResult.Right = mRotationY * ((mRotationX * (mRotationZ * aResult.Right)));

        return aResult;
    }

    //! Transfert d'un axe dans le repère d'un autre axe
    inline CAxis transferTo(CAxis aTarget) const
    {
        CAxis aResult(*this);

        CVector3 vAngles = aTarget.euleurAngles();

        CMatrix4 mRotationX = CMatrix4::MakeRotation(CVector3(vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::MakeRotation(CVector3(0.0, vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::MakeRotation(CVector3(0.0, 0.0, vAngles.Z));

        aResult.Front = mRotationY * ((mRotationX * (mRotationZ * aResult.Front)));
        aResult.Up = mRotationY * ((mRotationX * (mRotationZ * aResult.Up)));
        aResult.Right = mRotationY * ((mRotationX * (mRotationZ * aResult.Right)));

        return aResult;

        /*
            Matrix4 mTransform(Right, Up, Front);

            return Axis(mTransform * aTarget.Right, mTransform * aTarget.Up, mTransform * aTarget.Front);
            */
    }

    //! Transfert d'un axe depuis le repère d'un autre axe
    inline CAxis transferFrom(CAxis aTarget) const
    {
        CAxis aResult(*this);

        CVector3 vAngles = aTarget.euleurAngles();

        CMatrix4 mRotationX = CMatrix4::MakeRotation(CVector3(-vAngles.X, 0.0, 0.0));
        CMatrix4 mRotationY = CMatrix4::MakeRotation(CVector3(0.0, -vAngles.Y, 0.0));
        CMatrix4 mRotationZ = CMatrix4::MakeRotation(CVector3(0.0, 0.0, -vAngles.Z));

        aResult.Front = mRotationZ * ((mRotationX * (mRotationY * aResult.Front)));
        aResult.Up = mRotationZ * ((mRotationX * (mRotationY * aResult.Up)));
        aResult.Right = mRotationZ * ((mRotationX * (mRotationY * aResult.Right)));

        return aResult;
    }

    //! Multiplie un axe et une matrice
    inline CAxis operator * (const CMatrix4& mMatrix) const
    {
        CAxis aResult(*this);

        CVector3 vOrigin = mMatrix * CVector3();

        aResult.Front = (mMatrix * aResult.Front) - vOrigin;
        aResult.Up = (mMatrix * aResult.Up) - vOrigin;
        aResult.Right = (mMatrix * aResult.Right) - vOrigin;

        return aResult;
    }

    //! Récupération des angles euleur
    inline CVector3 euleurAngles() const
    {
        CVector3 vDiff1 = Front;
        CVector3 vDiff2 = Right;

        double dY = vDiff1.AngleY();

        CMatrix4 mRotationCancelPan = CMatrix4::MakeRotation(CVector3(0.0, vDiff1.AngleY() * -1.0, 0.0));

        vDiff1 = mRotationCancelPan * vDiff1;
        vDiff2 = mRotationCancelPan * vDiff2;

        double dX = vDiff1.AngleX();

        CMatrix4 mRotationCancelTilt = CMatrix4::MakeRotation(CVector3(vDiff1.AngleX() * -1.0, 0.0, 0.0));

        vDiff1 = mRotationCancelTilt * vDiff1;
        vDiff2 = mRotationCancelTilt * vDiff2;

        double dZ = vDiff2.AngleZ();

        dX = Math::Angles::clipAngleRadianPIMinusPI(dX);
        dY = Math::Angles::clipAngleRadianPIMinusPI(dY);
        dZ = Math::Angles::clipAngleRadianPIMinusPI(dZ);

        return CVector3(dX, dY, dZ);
    }
};

}

#endif // __AXIS_H__
