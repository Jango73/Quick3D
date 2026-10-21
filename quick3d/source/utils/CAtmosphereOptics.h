
#pragma once

// C++
#include <cmath>

// Application
#include "CVector3.h"
#include "Angles.h"

//-------------------------------------------------------------------------------------------------

/*!
    Analytic atmosphere optics shared by the sunlight tint and the sky model.

    Provides the Kasten and Young (1989) airmass plus Rayleigh and ozone
    (Chappuis bands) extinction without ray marching: optical paths stay
    analytic, \a dOzoneAmount scales the nominal ozone column (1.0 is the
    reference column). Both the CPU sunlight tint and the GPU sky shader
    implement these same equations, so keep them in sync when editing here.
*/
class CAtmosphereOptics
{
public:

    /************************************************************************/
    /*!
        Returns the relative airmass for \a dSinElevation, clamped to valid input.
    */
    static inline double computeAirmass(double dSinElevation)
    {
        if (dSinElevation <= 0.0)
        {
            dSinElevation = 0.0;
        }

        if (dSinElevation > 1.0)
        {
            dSinElevation = 1.0;
        }

        // Kasten and Young (1989) relative airmass coefficients
        const double dKastenYoungA = 0.50572;
        const double dKastenYoungB = 6.07995;
        const double dKastenYoungC = 1.6364;

        double dElevationDeg = Math::Angles::toDeg(asin(dSinElevation));

        return 1.0 / (dSinElevation + dKastenYoungA * pow(dElevationDeg + dKastenYoungB, -dKastenYoungC));
    }

    /************************************************************************/
    /*!
        Returns the per channel total extinction for the given Rayleigh beta,
        ozone beta and ozone amount. Free function so shaders and CPU stay consistent.
    */
    static inline Math::CVector3 computeExtinction(
        double dBetaRayleighR,
        double dBetaRayleighG,
        double dBetaRayleighB,
        double dBetaOzoneR,
        double dBetaOzoneG,
        double dBetaOzoneB,
        double dOzoneAmount)
    {
        if (dOzoneAmount < 0.0)
        {
            dOzoneAmount = 0.0;
        }

        return Math::CVector3(
            dBetaRayleighR + dOzoneAmount * dBetaOzoneR,
            dBetaRayleighG + dOzoneAmount * dBetaOzoneG,
            dBetaRayleighB + dOzoneAmount * dBetaOzoneB);
    }

    /************************************************************************/
    /*!
        Returns the sun transmittance for \a dSinElevation, before radiance scaling.
        Combines Rayleigh scattering and ozone absorption over the same slant path.
    */
    static inline Math::CVector3 computeSunTransmittance(
        double dSinElevation,
        double dScaleHeight,
        double dBetaRayleighR,
        double dBetaRayleighG,
        double dBetaRayleighB,
        double dBetaOzoneR,
        double dBetaOzoneG,
        double dBetaOzoneB,
        double dOzoneAmount)
    {
        if (dSinElevation <= 0.0)
        {
            return Math::CVector3(0.0, 0.0, 0.0);
        }

        double dPath = dScaleHeight * computeAirmass(dSinElevation);

        Math::CVector3 vExtinction = computeExtinction(
            dBetaRayleighR, dBetaRayleighG, dBetaRayleighB,
            dBetaOzoneR, dBetaOzoneG, dBetaOzoneB,
            dOzoneAmount);

        return Math::CVector3(
            exp(-vExtinction.X * dPath),
            exp(-vExtinction.Y * dPath),
            exp(-vExtinction.Z * dPath));
    }

    /************************************************************************/
    /*!
        Returns the share of extinction caused by Rayleigh scattering per channel.
        Weights the in-scattered sky light: absorbed light does not scatter, so
        ozone rich channels contribute less scattered light. Returns one per
        channel when \a dOzoneAmount is zero.
    */
    static inline Math::CVector3 computeScatteringShare(
        double dBetaRayleighR,
        double dBetaRayleighG,
        double dBetaRayleighB,
        double dBetaOzoneR,
        double dBetaOzoneG,
        double dBetaOzoneB,
        double dOzoneAmount)
    {
        Math::CVector3 vExtinction = computeExtinction(
            dBetaRayleighR, dBetaRayleighG, dBetaRayleighB,
            dBetaOzoneR, dBetaOzoneG, dBetaOzoneB,
            dOzoneAmount);

        Math::CVector3 vShare(1.0, 1.0, 1.0);

        if (vExtinction.X > 0.0)
        {
            vShare.X = dBetaRayleighR / vExtinction.X;
        }

        if (vExtinction.Y > 0.0)
        {
            vShare.Y = dBetaRayleighG / vExtinction.Y;
        }

        if (vExtinction.Z > 0.0)
        {
            vShare.Z = dBetaRayleighB / vExtinction.Z;
        }

        return vShare;
    }

    /************************************************************************/
    /*!
        Returns the Rayleigh phase value for \a dCosSunViewAngle.
    */
    static inline double computeRayleighPhase(double dCosSunViewAngle)
    {
        const double dPi = 3.141592653589793;

        return 3.0 / (16.0 * dPi) * (1.0 + dCosSunViewAngle * dCosSunViewAngle);
    }
};

