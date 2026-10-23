
// Application
#include "CWaterWaves.h"
#include "CPerlin.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the shader amplitude for the given wind level.
*/
/***************************************************************************/
double CWaterWaves::amplitudeFromWindLevel(double dWindLevel)
{
    return dWindLevel * WindToAmplitudeGain;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the wave height at a world origin relative position.
    \br\br
    Same formula as the waveHeight() GLSL function: two opposed turbulence
    samples scaled by the wind driven amplitude. The caller passes the world
    position minus the scene origin because the shader evaluates in the model
    matrix frame, which is already origin relative.
*/
/***************************************************************************/
double CWaterWaves::heightAt(const CVector3& vRelativePositionM, double dTimeS, double dWindLevel)
{
    CPerlin* pPerlin = CPerlin::getInstance();

    double dAmplitude = amplitudeFromWindLevel(dWindLevel);

    CVector3 vTimeOffset(dTimeS, dTimeS, dTimeS);

    CVector3 vPositionA = vRelativePositionM;
    CVector3 vPositionB = vRelativePositionM + CVector3(SecondSampleOffsetM, SecondSampleOffsetM, SecondSampleOffsetM);

    double dValueA = pPerlin->turbulence((vPositionA * SpatialScale) + vTimeOffset * TimeRate) * dAmplitude * HeightGain;
    double dValueB = pPerlin->turbulence((vPositionB * SpatialScale) - vTimeOffset * TimeRate) * dAmplitude * HeightGain;

    return dValueA + dValueB;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the water surface plane used by the Jolt buoyancy impulse.
    \br\br
    Height comes from heightAt(), the normal from finite differences along X
    and Z with the same orientation as the Jolt BoatTest sample: the cross
    product order keeps the normal in the up hemisphere. A calm sea returns
    a zero height and an exact up normal.
*/
/***************************************************************************/
void CWaterWaves::surfaceAt(const CVector3& vRelativePositionM, double dTimeS, double dWindLevel, double& dHeightM, CVector3& vNormal)
{
    dHeightM = heightAt(vRelativePositionM, dTimeS, dWindLevel);

    double dHeightXM = heightAt(vRelativePositionM + CVector3(SurfaceNormalEpsilonM, 0.0, 0.0), dTimeS, dWindLevel);
    double dHeightZM = heightAt(vRelativePositionM + CVector3(0.0, 0.0, SurfaceNormalEpsilonM), dTimeS, dWindLevel);

    CVector3 vTangentX(SurfaceNormalEpsilonM, dHeightXM - dHeightM, 0.0);
    CVector3 vTangentZ(0.0, dHeightZM - dHeightM, SurfaceNormalEpsilonM);

    vNormal = vTangentZ.cross(vTangentX).normalized();
}
