
#include "CBox.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------
// Constantes

#define BOX_SIZE	0.5

//-------------------------------------------------------------------------------------------------

CBox::CBox(C3DScene* pScene, double dMaxDistance)
    : CMesh(pScene, dMaxDistance)
    , m_vMinimum(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE)
    , m_vMaximum( BOX_SIZE,  BOX_SIZE,  BOX_SIZE)
{
    setName("Box");

    m_pGeometry->setMaterial(m_pScene->getRessourcesManager()->getDefaultMaterial());
    m_pGeometry->getMaterials()[0]->setIRFactor(0.4);

    fillVertices();
}

//-------------------------------------------------------------------------------------------------

CBox::~CBox()
{
}

//-------------------------------------------------------------------------------------------------

void CBox::setMinimum(const CVector3& vMinimum)
{
    m_vMinimum = vMinimum;

    fillVertices();
}

//-------------------------------------------------------------------------------------------------

void CBox::setMaximum(const CVector3& vMaximum)
{
    m_vMaximum = vMaximum;

    fillVertices();
}

//-------------------------------------------------------------------------------------------------

void CBox::setBounds(const CVector3& vMinimum, const CVector3& vMaximum)
{
    m_vMinimum = vMinimum;
    m_vMaximum = vMaximum;

    fillVertices();
}

//-------------------------------------------------------------------------------------------------

void CBox::fillVertices()
{
    m_pGeometry->createBox(m_vMinimum, m_vMaximum);
}
