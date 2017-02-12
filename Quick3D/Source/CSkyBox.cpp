
// Application
#include "C3DScene.h"
#include "CSkyBox.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define BOX_SIZE	1.0
#define COORD_STEP	((float) (1.0 / 4.0))

//-------------------------------------------------------------------------------------------------

CComponent* CSkyBox::instanciator(C3DScene* pScene)
{
    return new CSkyBox(pScene);
}

//-------------------------------------------------------------------------------------------------

CSkyBox::CSkyBox(C3DScene* pScene)
    : CBox(pScene)
{
    setCastShadows(false);
    setRaytracable(false);
    m_pGeometry->setMaterial(m_pScene->ressourcesManager()->getSkyboxMaterial());
    flipNormals();
}

//-------------------------------------------------------------------------------------------------

CSkyBox::~CSkyBox()
{
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::update(double dDeltaTime)
{
    CBox::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::paint(CRenderContext* pContext)
{
    double dScale = pContext->camera()->getMaxDistance() * 0.75;

    setOriginScale(CVector3(dScale, dScale, dScale));
    setWorldTransform(pContext->camera()->getWorldTransform());
    // setOriginPosition(pContext->camera()->getWorldPosition());
    // computeWorldTransform();

    CMesh::paint(pContext);
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CMesh::loadParameters(sBaseFile, xComponent);
}
