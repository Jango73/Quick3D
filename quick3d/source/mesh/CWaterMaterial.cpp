
#include "CWaterMaterial.h"
#include "C3DScene.h"
#include "CWaterWaves.h"

using namespace Math;

CWaterMaterial::CWaterMaterial(C3DScene* pScene)
    : CMaterial(pScene)
{
    m_cDiffuse = CVector4(0.05, 0.20, 0.10, 1.0);
    m_cSpecular = CVector4(0.8, 0.8, 0.8, 1.0);
    m_dShininess = 0.8;
    m_dMetalness = 0.6;
    m_dIRFactor = 0.2;
    m_dRigidness = 0.75;
    m_bHasAlpha = true;
}

CWaterMaterial::~CWaterMaterial()
{
}

void CWaterMaterial::applyStaticState(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    CMaterial::applyStaticState(pContext, pProgram);

    double dAmplitude = CWaterWaves::amplitudeFromWindLevel(m_pScene->windLevel());

    pProgram->setUniformValue("u_wave_enable", (GLint) 1);
    pProgram->setUniformValue("u_wave_amplitude", (GLfloat) (float) dAmplitude);
}

double CWaterMaterial::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness != nullptr)
    {
        *pRigidness = m_dRigidness;
    }

    return WaveHeight(gPosition);
}

double CWaterMaterial::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness)
{
    if (pRigidness != nullptr)
    {
        *pRigidness = m_dRigidness;
    }

    return WaveHeight(CGeoloc(vPosition));
}

double CWaterMaterial::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    return WaveHeight(CGeoloc(vPosition));
}

double CWaterMaterial::WaveHeight(CGeoloc gPosition)
{
    // Canonical definition lives in CWaterWaves::heightAt, shared with the
    // Jolt buoyancy query. Higher octave samples stay disabled on both sides.
    CVector3 vRelativePositionM = gPosition.toVector3() - m_pScene->worldOrigin();

    return CWaterWaves::heightAt(vRelativePositionM, m_dTime, m_pScene->windLevel());
}
