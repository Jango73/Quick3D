
#pragma once

// Std
#include "math.h"

// Application
#include "Angles.h"

//-------------------------------------------------------------------------------------------------

namespace Math
{
	struct Vector4
	{

	public:

		double X;
		double Y;
		double Z;
		double W;

		//! Default constructor
		inline Vector4 ()
		{
			X = 0.0;
			Y = 0.0;
			Z = 0.0;
			W = 0.0;
		}

		//! Constructeur par composants
		inline Vector4 (double NewX, double NewY, double NewZ, double NewW)
		{
			X = NewX;
			Y = NewY;
			Z = NewZ;
			W = NewW;
		}

		//! Constructeur de copie
		inline Vector4 (const Vector4& Target)
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
				if (*this != Vector4(0.0, 0.0, 0.0, 0.0))
				{
                    Vector4 Res = (*this) * (value / magnitude());

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
        inline Vector4 normalized() const
		{
            if (magnitude() != 0.0)
			{
                double inverse = 1.0 / magnitude();

				return Vector4
				(
					X * inverse,
					Y * inverse,
					Z * inverse,
					W * inverse
				);
			}

			return Vector4();
		}

		//! Renvoie la somme des composants
        inline double sumComponents() const
		{
			return (X + Y + Z + W);
		}

		//! Calcule la racine carrée des composants
        inline Vector4 sqrtComponents() const
		{
			return Vector4
			(
				sqrt(X),
				sqrt(Y),
				sqrt(Z),
				W
			);
		}

		//! Calcule le carré des composants
        inline Vector4 sqrComponents () const
		{
			return Vector4
			(
				X * X,
				Y * Y,
				Z * Z,
				W
			);
		}

		//! Calcule la somme des carrés des composants
        inline static double sumComponentSqrs (const Vector4& v1)
		{
            Vector4 v2 = v1.sqrComponents();

            return v2.sumComponents();
		}

		//! Calcule la somme des carrés des composants
        inline double sumComponentSqrs() const
		{
            return sumComponentSqrs(*this);
		}

		//! Opérateur d'égalité
		inline bool operator == (const Vector4& V2) const
		{
			return (X == V2.X) && (Y == V2.Y) && (Z == V2.Z) && (W == V2.W);
		}

		//! Opérateur d'inégalité
		inline bool operator != (const Vector4& V2) const
		{
			return !(*this == V2);
		}

		//! Opérateur d'assignation
		inline Vector4& operator = (const Vector4& Target)
		{
			X = Target.X;
			Y = Target.Y;
			Z = Target.Z;
			W = Target.W;

			return *this;
		}

		//! Opérateur d'addition
		inline Vector4 operator + (const Vector4& V2) const
		{
			return Vector4(X + V2.X, Y + V2.Y, Z + V2.Z, W + V2.W);
		}

		//! Opérateur de soustraction
		inline Vector4 operator - (const Vector4& V2) const
		{
			return Vector4(X - V2.X, Y - V2.Y, Z - V2.Z, W - V2.W);
		}

		//! Opérateur de multiplication
		inline Vector4 operator * (const Vector4& V2) const
		{
			return Vector4(X * V2.X, Y * V2.Y, Z * V2.Z, W * V2.W);
		}

		//! Opérateur d'addition
		Vector4 &operator += (const Vector4 &V2)
		{
			X += V2.X;
			Y += V2.Y;
			Z += V2.Z;
			W += V2.W;
			return *this;
		}

		//! Opérateur de soustraction
		Vector4 &operator -= (const Vector4 &V2)
		{
			X -= V2.X;
			Y -= V2.Y;
			Z -= V2.Z;
			W -= V2.W;
			return *this;
		}

		//! Opérateur de multiplication avec un double
		inline Vector4 operator * (double s2) const
		{
			return Vector4
			(
				X * s2,
				Y * s2,
				Z * s2,
				W * s2
			);
		}

		//! Opérateur de division avec un double
		inline Vector4 operator / (double s2) const
		{
			return Vector4
			(
				X / s2,
				Y / s2,
				Z / s2,
				W / s2
			);
		}

	    inline double &operator[](unsigned int index)
		{
			return (&X)[index];
		}

		inline const double &operator[](unsigned int index) const
		{
			return (&X)[index];
		}

        //! Cross product
        inline Vector4 cross(Vector4& v2) const
		{
			return Vector4
			(
				Y * v2.Z - Z * v2.Y,
				Z * v2.X - X * v2.Z,
				X * v2.Y - Y * v2.X,
				W
			);
		}

        //! Dot product
        inline double dot(const Vector4& v2) const
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
    };
}
