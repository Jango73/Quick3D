
#pragma once

// Std
#include "math.h"

// Qt
#include <QString>
#include <QDataStream>

// Application
#include "Angles.h"
#include "CVector2.h"

/*-------------------------------------------------------------------------------------------------
    Axis conventions

    The conventions used throughout the vector/matrix/geoloc classes in this library are those of the painter.

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
#define VECTOR_EQUALITY_EPSILON	0.0001

struct CVector3
{

public:

    double X;
    double Y;
    double Z;

    //! Default constructor
    inline CVector3()
    {
        X = 0.0;
        Y = 0.0;
        Z = 0.0;
    }

    //! Constructor with components
    inline CVector3(double newX, double newY, double newZ)
    {
        X = newX;
        Y = newY;
        Z = newZ;
    }

    //! Constructor with double scalar for all components
    inline CVector3(double value)
    {
        X = value;
        Y = value;
        Z = value;
    }

    //! Constructor with integer scalar for all components
    inline CVector3(int value)
    {
        X = (double) value;
        Y = (double) value;
        Z = (double) value;
    }

    //! Constructor with a 2D vector
    inline CVector3(const CVector2& value)
    {
        X = value.X;
        Y = value.Y;
        Z = 0.0;
    }

    //! Copy constructor
    inline CVector3(const CVector3& Target)
    {
        *this = Target;
    }

    //! Returns \c true if at least one component is not zero
    inline bool valid() const { return !(X == 0.0 && Y == 0.0 && Z == 0.0); }

    //!
    inline bool isSame(const CVector3& value)
    {
        return
                fabs(X - value.X) < VECTOR_EQUALITY_EPSILON &&
                fabs(Y - value.Y) < VECTOR_EQUALITY_EPSILON &&
                fabs(Z - value.Z) < VECTOR_EQUALITY_EPSILON;
    }

    //! Returns the length of this vector
    inline double magnitude () const
    {
        return sqrt(sumComponentSqrs());
    }

    //! Assigns a length to this vector
    inline void setMagnitude (double value)
    {
        if (value >= 0.0)
        {
            if (*this != CVector3(0.0, 0.0, 0.0))
            {
                CVector3 Res = (*this) * (value / magnitude());

                X = Res.X;
                Y = Res.Y;
                Z = Res.Z;
            }
        }
    }

    //! Returns \c true if this vector is of unit length
    inline bool isUnitVector()
    {
        return magnitude() == 1.0;
    }

    //! Returns a normalized version of this vector (length = 1.0)
    inline CVector3 normalized() const
    {
        if (magnitude() != 0.0)
        {
            double inverse = 1.0 / magnitude();

            return CVector3
                    (
                        X * inverse,
                        Y * inverse,
                        Z * inverse
                        );
        }

        return CVector3();
    }

    //! Returns the sum of this vector's components
    inline double sumComponents() const
    {
        return (X + Y + Z);
    }

    //! Returns this vector after computing the square root of each component
    inline CVector3 sqrtComponents() const
    {
        return CVector3
                (
                    sqrt(X),
                    sqrt(Y),
                    sqrt(Z)
                    );
    }

    //! Returns this vector after squaring each component
    inline CVector3 sqrComponents () const
    {
        return CVector3
                (
                    X * X,
                    Y * Y,
                    Z * Z
                    );
    }

    //! Calcule la somme des carrés des composants
    inline static double sumComponentSqrs (const CVector3& v1)
    {
        CVector3 v2 = v1.sqrComponents();

        return v2.sumComponents();
    }

    //! Calcule la somme des carrés des composants
    inline double sumComponentSqrs() const
    {
        return sumComponentSqrs(*this);
    }

    //! Returns \c true if this vector equals \a V2
    inline bool operator == (const CVector3& V2) const
    {
        return (X == V2.X) && (Y == V2.Y) && (Z == V2.Z);
    }

    //! Returns \c true if this vector does not equal \a V2
    inline bool operator != (const CVector3& V2) const
    {
        return !(*this == V2);
    }

    //! Returns \c true if this vector is less than \a V2
    inline bool operator < (const CVector3& V2) const
    {
        return (X < V2.X) || (Y < V2.Y) || (Z < V2.Z);
    }

    //! Returns \c true if this vector is greater than \a V2
    inline bool operator > (const CVector3& V2) const
    {
        return (X > V2.X) || (Y > V2.Y) || (Z > V2.Z);
    }

    //! Assign operator
    inline CVector3& operator = (const CVector3& Target)
    {
        X = Target.X;
        Y = Target.Y;
        Z = Target.Z;

        return *this;
    }

    //! Returns sum of this vector and \a V2
    inline CVector3 operator + (const CVector3& V2) const
    {
        return CVector3(X + V2.X, Y + V2.Y, Z + V2.Z);
    }

    //! Returns subtraction of this vector and \a V2
    inline CVector3 operator - (const CVector3& V2) const
    {
        return CVector3(X - V2.X, Y - V2.Y, Z - V2.Z);
    }

    //! Returns multiplication of this vector and \a V2
    inline CVector3 operator * (const CVector3& V2) const
    {
        return CVector3(X * V2.X, Y * V2.Y, Z * V2.Z);
    }

    //! Returns division of this vector and \a V2
    inline CVector3 operator / (const CVector3& V2) const
    {
        return CVector3(X / V2.X, Y / V2.Y, Z / V2.Z);
    }

    //! Returns sum of this vector and \a V2
    CVector3& operator += (const CVector3& V2)
    {
        X += V2.X;
        Y += V2.Y;
        Z += V2.Z;
        return *this;
    }

    //! Returns subtraction of this vector and \a V2
    CVector3& operator -= (const CVector3& V2)
    {
        X -= V2.X;
        Y -= V2.Y;
        Z -= V2.Z;
        return *this;
    }

    //! Returns multiplication of this vector and \a V2
    CVector3& operator *= (const CVector3& V2)
    {
        X *= V2.X;
        Y *= V2.Y;
        Z *= V2.Z;
        return *this;
    }

    //! Divide operator
    CVector3& operator /= (const CVector3& V2)
    {
        X /= V2.X;
        Y /= V2.Y;
        Z /= V2.Z;
        return *this;
    }

    //! Scalar multiply operator
    inline CVector3 operator * (double s2) const
    {
        return CVector3
                (
                    X * s2,
                    Y * s2,
                    Z * s2
                    );
    }

    //! Scalar divide operator
    inline CVector3 operator / (double s2) const
    {
        return CVector3
                (
                    X / s2,
                    Y / s2,
                    Z / s2
                    );
    }

    //! Scalar divide operator
    inline CVector3 operator / (int s2) const
    {
        return CVector3
                (
                    X / (double) s2,
                    Y / (double) s2,
                    Z / (double) s2
                    );
    }

    // Index operator
    inline double &operator[](unsigned int index)
    {
        return (&X)[index];
    }

    // Constant index operator
    inline const double &operator[](unsigned int index) const
    {
        return (&X)[index];
    }

    //! Cross product
    inline CVector3 cross(CVector3& v2) const
    {
        return CVector3
                (
                    Y * v2.Z - Z * v2.Y,
                    Z * v2.X - X * v2.Z,
                    X * v2.Y - Y * v2.X
                    );
    }

    //! Dot product
    inline double dot(const CVector3& v2) const
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

    //! Transforms a angle vector from degrees to radians
    inline CVector3 degreesToRadians()
    {
        return CVector3(
                    Angles::toRad(X),
                    Angles::toRad(Y),
                    Angles::toRad(Z)
                    );
    }

    //! Transforms a angle vector from radians to degrees
    inline CVector3 radiansToDegrees()
    {
        return CVector3(
                    Angles::toDeg(X),
                    Angles::toDeg(Y),
                    Angles::toDeg(Z)
                    );
    }

    //! Returns a string representation of this vector
    inline QString toString()
    {
        return QString("<%1, %2, %3>").arg(X).arg(Y).arg(Z);
    }

    friend CVector3 interpolate(const CVector3& v1, const CVector3& v2, double dValue);

    friend QDataStream& operator << (QDataStream& out, const CVector3&);
    friend QDataStream& operator >> (QDataStream& in, CVector3&);
};

inline CVector3 interpolate(const CVector3& v1, const CVector3& v2, double dValue)
{
    CVector3 vResult;

    vResult.X = v1.X + (v2.X - v1.X) * dValue;
    vResult.Y = v1.Y + (v2.Y - v1.Y) * dValue;
    vResult.Z = v1.Z + (v2.Z - v1.Z) * dValue;

    return vResult;
}

inline QDataStream& operator << (QDataStream& out, const CVector3& vValue)
{
    out << vValue.X;
    out << vValue.Y;
    out << vValue.Z;

    return out;
}

inline QDataStream& operator >> (QDataStream& in, CVector3& vValue)
{
    in >> vValue.X;
    in >> vValue.Y;
    in >> vValue.Z;

    return in;
}

}
