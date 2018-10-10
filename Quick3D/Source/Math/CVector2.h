
#pragma once

// Std
#include "math.h"

// Qt
#include <QSize>
#include <QPointF>

// Application
#include "Angles.h"

namespace Math
{
	struct CVector2
	{

	public:

		double X;
		double Y;

		//! Default constructor
        inline CVector2()
		{
			X = 0.0f;
			Y = 0.0f;
		}

		//! Constructeur par composants
        inline CVector2(double NewX, double NewY)
		{
			X = NewX;
			Y = NewY;
		}

		//! Constructeur par valeur
        inline CVector2(double value)
		{
			X = value;
			Y = value;
		}

		//! Constructeur de copie
        inline CVector2(const CVector2& Target)
		{
			X = Target.X;
			Y = Target.Y;
		}

		//! Renvoie la longueur du vecteur
        inline double magnitude() const
		{
            return sqrt(sumComponentSqrs());
		}

		//! Assigne la longueur du vecteur
        inline void setMagnitude(double value)
		{
			if (value >= 0.0f)
			{
				if (*this != CVector2(0.0f, 0.0f))
				{
                    CVector2 Res = (*this) * (value / magnitude());

					X = Res.X;
					Y = Res.Y;
				}
			}
		}

		//! Renvoie vrai si le vecteur est unitaire
        inline bool isUnitVector()
		{
            return magnitude() == 1.0f;
		}

		//! Normalise le vecteur = longueur de 1.0
        inline CVector2 normalized() const
		{
            if (magnitude() != 0.0f)
			{
                double inverse = 1.0f / magnitude();

				return CVector2(X * inverse, Y * inverse);
			}

			return CVector2();
		}

		//! Renvoie la somme des composants
        inline double sumComponents() const
		{
			return (X + Y);
		}

		//! Calcule la racine carrée des composants
        inline CVector2 sqrtComponents() const
		{
			return CVector2(sqrt(X), sqrt(Y));
		}

		//! Calcule le carré des composants
        inline CVector2 sqrComponents () const
		{
			return CVector2(X * X, Y * Y);
		}

        inline static double sumComponentSqrs (const CVector2& v1)
		{
            CVector2 v2 = v1.sqrComponents();

            return v2.sumComponents();
		}

        inline double sumComponentSqrs() const
		{
            return sumComponentSqrs(*this);
		}

		inline bool operator == (const CVector2& V2) const
		{
			return (X == V2.X) && (Y == V2.Y);
		}

		inline bool operator != (const CVector2& V2) const
		{
			return !(*this == V2);
		}

		inline bool operator < (const CVector2& V2) const
		{
			return (X < V2.X) || (Y < V2.Y);
		}

		inline bool operator > (const CVector2& V2) const
		{
			return (X > V2.X) || (Y > V2.Y);
		}

        //! Returns sum of this vector and \a V2
        inline CVector2 operator + (const CVector2& V2)
		{
			return CVector2(X + V2.X, Y + V2.Y);
		}

        //! Returns subtraction of this vector and \a V2
        inline CVector2 operator - (const CVector2& V2)
		{
			return CVector2(X - V2.X, Y - V2.Y);
		}

        //! Returns multiplication of this vector and \a V2
        inline CVector2 operator * (const CVector2& V2)
		{
			return CVector2(X * V2.X, Y * V2.Y);
		}

        //! Returns multiplication of this vector and the scalar \a s2
        inline CVector2 operator * (double s2)
		{
			return CVector2(X * s2, Y * s2);
		}

        //! Returns division of this vector and \a V2
        inline CVector2 operator / (const CVector2& V2)
		{
			return CVector2(X / V2.X, Y / V2.Y);
		}

        //! Returns division of this vector and the scalar \a s2
        inline CVector2 operator / (double s2)
		{
			return CVector2(X / s2, Y / s2);
		}

        //! Returns sum of this vector and \a V2
        CVector2& operator += (const CVector2& V2)
		{
			X += V2.X;
			Y += V2.Y;
			return *this;
		}

        // Index operator
		inline double& operator[](unsigned int index)
		{
			return (&X)[index];
		}

        // Constant index operator
        inline const double& operator[](unsigned int index) const
		{
			return (&X)[index];
		}

        //! Cross product
        inline CVector2 cross(CVector2& v2) const
		{
			return CVector2
			(
				Y * v2.X - X * v2.Y,
				X * v2.Y - Y * v2.X
			);
		}

        //! Dot product
        inline double dot(const CVector2& v2) const
		{
			return (X * v2.X + Y * v2.Y);
		}

        //! Returns the euler angle
        inline double eulerAngle() const
		{
			if (X == 0.0f) return 0.0f;
			return atan2(Y, X);
		}

        //! Transforms this vector to a QSize
		inline QSize toQSize() const
		{
			return QSize(X, Y);
		}

        //! Transforms this vector to a QPointF
		inline QPointF toQPointF() const
		{
			return QPointF(X, Y);
		}

        //! Returns a vertical FOV given an image size and a horizontal FOV
		static double computeVerticalFOV(const CVector2& imageSize, double dCameraHorizontalFOVDegrees)
		{
			return dCameraHorizontalFOVDegrees * (imageSize.Y / imageSize.X);
		}

        //! Returns a horizontal FOV given an image size and a vertical FOV
        static double computeHorizontalFOV(const CVector2& imageSize, double dCameraVerticalFOVDegrees)
        {
            return dCameraVerticalFOVDegrees * (imageSize.X / imageSize.Y);
        }

        //! Converts a point to angles
        static CVector2 pointToAngles(const CVector2& imageSize, double dCameraVerticalFOVDegrees, const CVector2& normalized2DPoint)
		{
            double dCameraFOVH = dCameraVerticalFOVDegrees;
            double dCameraFOVW = computeHorizontalFOV(imageSize, dCameraVerticalFOVDegrees);

			double dPixX = normalized2DPoint.X - 0.5;
			double dPixY = normalized2DPoint.Y - 0.5;

            double dLW = 0.5 / tan(Angles::toRad(dCameraFOVW / 2.0));
			double dLH = 0.5 / tan(Angles::toRad(dCameraFOVH / 2.0));

			double dRadX = atan(dPixX / dLW);
			double dRadY = atan(dPixY / dLH);

			return CVector2(dRadX, dRadY);
		}
    };
}
