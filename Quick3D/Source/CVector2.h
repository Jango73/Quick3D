
#ifndef __VECTOR2_H__
#define __VECTOR2_H__

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
		inline CVector2 ()
		{
			X = 0.0f;
			Y = 0.0f;
		}

		//! Constructeur par composants
		inline CVector2 (double NewX, double NewY)
		{
			X = NewX;
			Y = NewY;
		}

		//! Constructeur par valeur
		inline CVector2 (double value)
		{
			X = value;
			Y = value;
		}

		//! Constructeur de copie
		inline CVector2 (const CVector2& Target)
		{
			X = Target.X;
			Y = Target.Y;
		}

		//! Renvoie la longueur du vecteur
		inline double getMagnitude () const
		{
			return sqrt(SumComponentSqrs());
		}

		//! Assigne la longueur du vecteur
		inline void setMagnitude (double value)
		{
			if (value >= 0.0f)
			{
				if (*this != CVector2(0.0f, 0.0f))
				{
					CVector2 Res = (*this) * (value / getMagnitude());

					X = Res.X;
					Y = Res.Y;
				}
			}
		}

		//! Renvoie vrai si le vecteur est unitaire
		inline bool IsUnitVector()
		{
			return getMagnitude() == 1.0f;
		}

		//! Normalise le vecteur = longueur de 1.0
		inline CVector2 Normalize()
		{
			if (getMagnitude() != 0.0f)
			{
				double inverse = 1.0f / getMagnitude();

				return CVector2(X * inverse, Y * inverse);
			}

			return CVector2();
		}

		//! Renvoie la somme des composants
		inline double SumComponents() const
		{
			return (X + Y);
		}

		//! Calcule la racine carrée des composants
		inline CVector2 SqrtComponents() const
		{
			return CVector2(sqrt(X), sqrt(Y));
		}

		//! Calcule le carré des composants
		inline CVector2 SqrComponents () const
		{
			return CVector2(X * X, Y * Y);
		}

		inline static double SumComponentSqrs (const CVector2& v1)
		{
			CVector2 v2 = v1.SqrComponents();

			return v2.SumComponents();
		}

		inline double SumComponentSqrs() const
		{
			return SumComponentSqrs(*this);
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

		//! Opérateur d'addition
		inline CVector2 operator + (const CVector2& V2)
		{
			return CVector2(X + V2.X, Y + V2.Y);
		}

		//! Opérateur de soustraction
		inline CVector2 operator - (const CVector2& V2)
		{
			return CVector2(X - V2.X, Y - V2.Y);
		}

		//! Opérateur de multiplication
		inline CVector2 operator * (const CVector2& V2)
		{
			return CVector2(X * V2.X, Y * V2.Y);
		}

		//! Opérateur de multiplication
		inline CVector2 operator * (double s2)
		{
			return CVector2(X * s2, Y * s2);
		}

		//! Opérateur de division
		inline CVector2 operator / (const CVector2& V2)
		{
			return CVector2(X / V2.X, Y / V2.Y);
		}

		//! Opérateur de division
		inline CVector2 operator / (double s2)
		{
			return CVector2(X / s2, Y / s2);
		}

		//! Opérateur d'addition
		CVector2& operator += (const CVector2& V2)
		{
			X += V2.X;
			Y += V2.Y;
			return *this;
		}

		inline double& operator[](unsigned int index)
		{
			return (&X)[index];
		}

		inline const double& operator[](unsigned int index) const
		{
			return (&X)[index];
		}

		//! Produit vectoriel
		inline CVector2 CrossProduct(CVector2& v2) const
		{
			return CVector2
			(
				Y * v2.X - X * v2.Y,
				X * v2.Y - Y * v2.X
			);
		}

		//! Produit scalaire
		inline double DotProduct(const CVector2& v2) const
		{
			return (X * v2.X + Y * v2.Y);
		}

		// Renvoie l'angle
		inline double Angle() const
		{
			if (X == 0.0f) return 0.0f;
			return atan2(Y, X);
		}

		//
		inline QSize toQSize() const
		{
			return QSize(X, Y);
		}

		//
		inline QPointF toQPointF() const
		{
			return QPointF(X, Y);
		}

		static double computeVerticalFOV(const CVector2& imageSize, double dCameraHorizontalFOVDegrees)
		{
			return dCameraHorizontalFOVDegrees * (imageSize.Y / imageSize.X);
		}

		//! Convertit un point en angles
		static CVector2 pointToAngles(const CVector2& imageSize, double dCameraHorizontalFOVDegrees, const CVector2& normalized2DPoint)
		{
			double dCameraFOVW = dCameraHorizontalFOVDegrees;
			double dCameraFOVH = computeVerticalFOV(imageSize, dCameraHorizontalFOVDegrees);

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

#endif // __VECTOR2_H__
