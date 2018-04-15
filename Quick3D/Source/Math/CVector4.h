
#pragma once

// Std
#include "math.h"

// Qt
#include <QString>

// Application
#include "Angles.h"

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
struct CVector4
{

public:

    double X;
    double Y;
    double Z;
    double W;

    //! Default constructor
    inline CVector4 ()
    {
        X = 0.0;
        Y = 0.0;
        Z = 0.0;
        W = 0.0;
    }

    //! Constructor with components
    inline CVector4 (double newX, double newY, double newZ, double newW)
    {
        X = newX;
        Y = newY;
        Z = newZ;
        W = newW;
    }

    //! Constructor with double scalar for all components
    inline CVector4(double value)
    {
        X = value;
        Y = value;
        Z = value;
        W = value;
    }

    //! Constructor with double scalar for all components
    inline CVector4(int value)
    {
        X = (double) value;
        Y = (double) value;
        Z = (double) value;
        W = (double) value;
    }

    //! Copy constructor
    inline CVector4 (const CVector4& Target)
    {
        *this = Target;
    }

    //! Renvoie la longueur du vecteur
    inline double magnitude () const
    {
        return sqrt(sumComponentSqrs());
    }

    //! Assigne la longueur du vecteur
    inline void setMagnitude (double value)
    {
        if (value >= 0.0)
        {
            if (*this != CVector4(0.0, 0.0, 0.0, 0.0))
            {
                CVector4 Res = (*this) * (value / magnitude());

                X = Res.X;
                Y = Res.Y;
                Z = Res.Z;
                W = Res.W;
            }
        }
    }

    //! Renvoie vrai si le vecteur est unitaire
    inline bool isUnitVector()
    {
        return magnitude() == 1.0;
    }

    //! Normalise le vecteur = longueur de 1.0
    inline CVector4 normalized() const
    {
        if (magnitude() != 0.0)
        {
            double inverse = 1.0 / magnitude();

            return CVector4
                    (
                        X * inverse,
                        Y * inverse,
                        Z * inverse,
                        W * inverse
                        );
        }

        return CVector4();
    }

    //! Renvoie la somme des composants
    inline double sumComponents() const
    {
        return (X + Y + Z + W);
    }

    //! Calcule la racine carrée des composants
    inline CVector4 sqrtComponents() const
    {
        return CVector4
                (
                    sqrt(X),
                    sqrt(Y),
                    sqrt(Z),
                    W
                    );
    }

    //! Calcule le carré des composants
    inline CVector4 sqrComponents () const
    {
        return CVector4
                (
                    X * X,
                    Y * Y,
                    Z * Z,
                    W
                    );
    }

    //! Calcule la somme des carrés des composants
    inline static double sumComponentSqrs (const CVector4& v1)
    {
        CVector4 v2 = v1.sqrComponents();

        return v2.sumComponents();
    }

    //! Calcule la somme des carrés des composants
    inline double sumComponentSqrs() const
    {
        return sumComponentSqrs(*this);
    }

    //! Opérateur d'égalité
    inline bool operator == (const CVector4& V2) const
    {
        return (X == V2.X) && (Y == V2.Y) && (Z == V2.Z) && (W == V2.W);
    }

    //! Opérateur d'inégalité
    inline bool operator != (const CVector4& V2) const
    {
        return !(*this == V2);
    }

    //! Assign operator
    inline CVector4& operator = (const CVector4& Target)
    {
        X = Target.X;
        Y = Target.Y;
        Z = Target.Z;
        W = Target.W;

        return *this;
    }

    //! Returns sum of this vector and \a V2
    inline CVector4 operator + (const CVector4& V2) const
    {
        return CVector4(X + V2.X, Y + V2.Y, Z + V2.Z, W + V2.W);
    }

    //! Returns subtraction of this vector and \a V2
    inline CVector4 operator - (const CVector4& V2) const
    {
        return CVector4(X - V2.X, Y - V2.Y, Z - V2.Z, W - V2.W);
    }

    //! Returns multiplication of this vector and \a V2
    inline CVector4 operator * (const CVector4& V2) const
    {
        return CVector4(X * V2.X, Y * V2.Y, Z * V2.Z, W * V2.W);
    }

    //! Returns division of this vector and \a V2
    inline CVector4 operator / (const CVector4& V2) const
    {
        return CVector4(X / V2.X, Y / V2.Y, Z / V2.Z, W / V2.W);
    }

    //! Returns sum of this vector and \a V2
    inline CVector4& operator += (const CVector4& V2)
    {
        X += V2.X;
        Y += V2.Y;
        Z += V2.Z;
        W += V2.W;

        return *this;
    }

    //! Returns subtraction of this vector and \a V2
    inline CVector4& operator -= (const CVector4& V2)
    {
        X -= V2.X;
        Y -= V2.Y;
        Z -= V2.Z;
        W -= V2.W;

        return *this;
    }

    //! Returns multiplication of this vector and \a V2
    inline CVector4& operator *= (const CVector4& V2)
    {
        X *= V2.X;
        Y *= V2.Y;
        Z *= V2.Z;
        W *= V2.W;

        return *this;
    }

    //! Returns division of this vector and \a V2
    inline CVector4& operator /= (const CVector4& V2)
    {
        X /= V2.X;
        Y /= V2.Y;
        Z /= V2.Z;
        W /= V2.W;

        return *this;
    }

    //! Returns multiplication of this vector and \a value
    inline CVector4 operator * (double value) const
    {
        return CVector4
                (
                    X * value,
                    Y * value,
                    Z * value,
                    W * value
                    );
    }

    //! Returns division of this vector and \a value
    inline CVector4 operator / (double value) const
    {
        return CVector4
                (
                    X / value,
                    Y / value,
                    Z / value,
                    W / value
                    );
    }

    //! Returns component \a index of this vector
    inline double& operator[] (unsigned int index)
    {
        return (&X)[index];
    }

    //! Returns component \a index of this vector
    inline const double& operator[] (unsigned int index) const
    {
        return (&X)[index];
    }

    //! Cross product
    inline CVector4 cross(CVector4& v2) const
    {
        return CVector4
                (
                    Y * v2.Z - Z * v2.Y,
                    Z * v2.X - X * v2.Z,
                    X * v2.Y - Y * v2.X,
                    W
                    );
    }

    //! Dot product
    inline double dot(const CVector4& v2) const
    {
        return
                (
                    X * v2.X +
                    Y * v2.Y +
                    Z * v2.Z
                    );
    }

    //! Returns the euler X angle
    inline double eulerXAngle() const
    {
        if (Y == 0.0) return 0.0;
        return atan2(Z, Y) - (Math::Pi / 2.0);
    }

    //! Returns the euler Y angle
    inline double eulerYAngle() const
    {
        if (Z == 0.0)
        {
            if (X == 0.0) return 0.0;
            return atan2(Z, X) + (Math::Pi / 2.0);
        }
        return atan2(X, Z);
    }

    //! Returns the euler Z angle
    inline double eulerZAngle() const
    {
        if (X == 0.0) return 0.0;
        return atan2(Y, X);
    }

    //! Returns a string representation of this vector
    inline QString toString() const
    {
        return QString("<%1, %2, %3, %4>").arg(X).arg(Y).arg(Z).arg(W);
    }
};
}
