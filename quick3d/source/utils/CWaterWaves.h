
//-------------------------------------------------------------------------------------------------
// Canonical CPU definition of the sea surface displaced by the vertex shaders.
//
// VS_Standard.c (glsl1 and glsl4) implements the same waveHeight() formula in
// GLSL: keep SpatialScale, TimeRate, SecondSampleOffsetM, HeightGain and
// WindToAmplitudeGain in sync on both sides. CTestWaterWaves locks the
// constants against the shader sources.
//-------------------------------------------------------------------------------------------------

#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWaterWaves
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------------------------------

    //! Spatial frequency of the turbulence samples, mirrors the shader 0.005 factor
    static constexpr double SpatialScale = 0.005;

    //! Time drift rate of the turbulence samples, mirrors the shader 0.1 factor
    static constexpr double TimeRate = 0.1;

    //! Offset in meters of the second turbulence sample, mirrors the shader vec3(2.0)
    static constexpr double SecondSampleOffsetM = 2.0;

    //! Height gain applied to each turbulence sample, mirrors the shader 4.0 factor
    static constexpr double HeightGain = 4.0;

    //! Wind level to shader amplitude gain, shared by the CPU query and u_wave_amplitude
    static constexpr double WindToAmplitudeGain = 4.0;

    //! Finite difference step in meters for the surface normal, Jolt buoyancy needs the plane
    static constexpr double SurfaceNormalEpsilonM = 1.0;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the shader amplitude for \a dWindLevel, the u_wave_amplitude uniform value
    static double amplitudeFromWindLevel(double dWindLevel);

    //! Returns the wave height in meters at \a vRelativePositionM (world minus origin) for \a dTimeS and \a dWindLevel
    static double heightAt(const Math::CVector3& vRelativePositionM, double dTimeS, double dWindLevel);

    //! Returns the Jolt buoyancy plane at \a vRelativePositionM: height in \a dHeightM and unit up normal in \a vNormal
    static void surfaceAt(const Math::CVector3& vRelativePositionM, double dTimeS, double dWindLevel, double& dHeightM, Math::CVector3& vNormal);
};
