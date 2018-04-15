
// Application
#include "C3DScene.h"
#include "CRain.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define MAX_PARTICLES 1000

//-------------------------------------------------------------------------------------------------

CRain::CRain(C3DScene* pScene)
    : CParticleSystem(pScene)
{
    m_pGeometry->setMaterial(m_pScene->ressourcesManager()->getDefaultMaterial());
    m_pGeometry->materials()[0]->diffuse() = CVector4(0.8, 0.8, 1.0, 0.4);
    m_pGeometry->materials()[0]->specular() = CVector4(0.4, 0.4, 0.4, 1.0);
    m_pGeometry->materials()[0]->setShininess(0.8);
    m_pGeometry->materials()[0]->setIRFactor(0.4);
}

//-------------------------------------------------------------------------------------------------

CRain::~CRain()
{
}

//-------------------------------------------------------------------------------------------------

void CRain::update(double dDeltaTime)
{
    CParticleSystem::update(dDeltaTime);

    double dSpeed = 80.0;
    CVector3 vVelocity(0.0, -dSpeed, 0.0);

    int iIndex = 0;

    for (; iIndex < m_vParticles.count(); iIndex++)
    {
        m_vParticles[iIndex].position() = m_vParticles[iIndex].position() + (m_vParticles[iIndex].velocity() * dDeltaTime);

        if (m_vParticles[iIndex].position().Y < 0.0)
        {
            m_vParticles.remove(iIndex);
            iIndex--;
        }
    }

    for (int iAddIndex = iIndex; iAddIndex < MAX_PARTICLES; iAddIndex++)
    {
        double randX = rand() / (double) RAND_MAX;
        double randY = rand() / (double) RAND_MAX;
        double randZ = rand() / (double) RAND_MAX;

        CVector3 vPosition((randX * 200.0) - 100.0, (randY * 100.0), randZ * 500.0);

        m_vParticles.append(CParticle(vPosition, vVelocity));
    }
}

//-------------------------------------------------------------------------------------------------

void CRain::paint(CRenderContext* pContext)
{
    setPosition(CVector3(pContext->camera()->worldPosition().X, 0.0, pContext->camera()->worldPosition().Z));

    CParticleSystem::paint(pContext);
}
