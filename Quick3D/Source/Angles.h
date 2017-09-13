
#pragma once

// Std
#include "math.h"

// Qt
#include <QSize>
#include <QPointF>

namespace Math
{
//! Constante de PI sur 21 chiffres
const double Pi		= 3.14159265358979323846;

//! Constante de racine carrée de 2 sur 21 chiffres
const double Sqrt2	= 1.41421356237309504880;

//! Constante de PI x 2
const double _2Pi	= Pi * 2.0;

class Angles
{
public:

    //! Retourne le signe d'un nombre
    static inline int sign(double dValue)
    {
        return dValue < 0.0 ? -1.0 : 1.0;
    }

    //! Limite un int dans la zone dMin -> dMax
    static int clipInt(int iValue, int iMin, int iMax)
    {
        if (iValue < iMin) iValue = iMin;
        if (iValue > iMax) iValue = iMax;

        return iValue;
    }

    //! Limite un double dans la zone dMin -> dMax
    static double clipDouble(double dValue, double dMin, double dMax)
    {
        if (dValue < dMin) dValue = dMin;
        if (dValue > dMax) dValue = dMax;

        return dValue;
    }

    //! Limite un angle en degrés dans la zone 0 -> 360
    static double clipAngleDegree(double dAngle)
    {
        while (dAngle <   0.0) dAngle += 360.0;
        while (dAngle > 360.0) dAngle -= 360.0;

        return dAngle;
    }

    //! Limite un angle en radians dans la zone 0 -> 2 PI
    static double clipAngleRadian(double dAngle)
    {
        while (dAngle <   0.0) dAngle += _2Pi;
        while (dAngle >= _2Pi) dAngle -= _2Pi;

        return dAngle;
    }

    //! Limite un angle en degrés dans la zone -180 -> +180
    static double clipAngleDegreePIMinusPI(double dAngle)
    {
        while (dAngle < -180.0) dAngle += 360.0;
        while (dAngle >  180.0) dAngle -= 360.0;

        return dAngle;
    }

    //! Limite un angle en radians dans la zone -PI -> +PI
    static double clipAngleRadianPIMinusPI(double dAngle)
    {
        while (dAngle < -Pi) dAngle += _2Pi;
        while (dAngle >  Pi) dAngle -= _2Pi;

        return dAngle;
    }

    //! Limite un angle en degrés dans la zone dLow -> dHigh
    static double limitAngleDegree(double dAngle, double dLow, double dHigh)
    {
        if (dAngle < dLow) dAngle = dLow;
        if (dAngle > dHigh) dAngle = dHigh;

        return dAngle;
    }

    //! Limite un angle en degrés dans la zone dLow -> dHigh
    static double limitAngleDegreeClosest(double dAngle, double dLow, double dHigh)
    {
        if (dAngle < dLow || dAngle > dHigh)
        {
            double dDeltaLow1 = fabs(dAngle - dLow);
            double dDeltaLow2 = (dLow == 0.0) ? fabs(dAngle - 360.0) : 9999.0;
            double dDeltaHigh = fabs(dAngle - dHigh);

            if (dDeltaLow1 < dDeltaHigh || dDeltaLow2 < dDeltaHigh)
            {
                dAngle = dLow;
            }
            else dAngle = dHigh;
        }

        return dAngle;
    }

    //! Returns shortest angle in degrees between dAngle1 and dAngle2
    static double angleDifferenceDegree(double dAngle1, double dAngle2)
    {
        double a = dAngle1 - dAngle2;
        a += (a > 180) ? -360 : (a < -180) ? 360 : 0;
        return a;
    }

    //! Returns shortest angle in radians between dAngle1 and dAngle2
    static double angleDifferenceRadian(double dAngle1, double dAngle2)
    {
        double a = dAngle1 - dAngle2;
        a += (a > Pi) ? -(Pi * 2.0) : (a < -Pi) ? (Pi * 2.0) : 0.0;
        return a;
    }

    //! Converts degrees to radians
    static double toRad(const double angleDeg)
    {
        return (angleDeg / 360.) * _2Pi;
    }

    //! Converts radians to degrees
    static double toDeg(const double angleRad)
    {
        return (angleRad / _2Pi) * 360.;
    }

    //! Convertit un angle radians en millièmes sens horaire
    static double toMil(const double angleRad)
    {
        return (clipAngleRadian(angleRad) / _2Pi) * 6400;
    }

    //! Convertit un angle millièmes en radians sens horaire
    static double miltoRad(const double angleMil)
    {
        return (angleMil / 6400.) * _2Pi;
    }

    //! Convertit un angle degrés en millièmes
    static int degToMil(const double angleDeg)
    {
        return (int) ((clipAngleDegree(angleDeg) / 360.0) * 6400.0);
    }

    //! Convertit un angle millièmes en degrés
    static double milToDeg(const int angleMil)
    {
        return ((double) angleMil / 6400.0) * 360.0;
    }

    //! Convertit un angle radians en grades
    static double radToGrad(double dRad)
    {
        return (dRad * 180.0) / Pi;
    }

    //! Convertit un angle grades en radians
    static double gradToRad(double dGrad)
    {
        return (dGrad / 180.0) * Pi;
    }

    //! Retourne la plus petite valeur
    static double _min(double value1, double value2)
    {
        if (value1 < value2)
            return value1;

        return value2;
    }

    //! Retourne la plus grande valeur
    static double _max(double value1, double value2)
    {
        if (value1 > value2)
            return value1;

        return value2;
    }

    static bool isEqual(double dValue1, double dValue2)
    {
        return fabs(dValue1 - dValue2) < 0.0001;
    }
};
}
