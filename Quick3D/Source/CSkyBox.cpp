
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
: CMesh(pScene)
{
	setRaytracable(false);
	setMaterial(m_pScene->getRessourcesManager()->getSkyboxMaterial());

	setCastShadows(false);

	// Front
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));

	// Top
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));

	// Right
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 3.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 3.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));

	// Left
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 0.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 0.0, COORD_STEP * 1.0)));

	// Back
	m_vVertices.append(CVertex(CVector3( BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE,  BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 4.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 4.0)));

	// Bottom
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE,  BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3( BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 2.0, COORD_STEP * 0.0)));
	m_vVertices.append(CVertex(CVector3(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE), CVector2(COORD_STEP * 1.0, COORD_STEP * 0.0)));

	m_vFaces.append(CFace(this,  3,  2,  1,  0));
	m_vFaces.append(CFace(this,  7,  6,  5,  4));
	m_vFaces.append(CFace(this, 11, 10,  9,  8));
	m_vFaces.append(CFace(this, 15, 14, 13, 12));
	m_vFaces.append(CFace(this, 19, 18, 17, 16));
	m_vFaces.append(CFace(this, 23, 22, 21, 20));

	updateGeometry();
}

//-------------------------------------------------------------------------------------------------

CSkyBox::~CSkyBox()
{
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::update(double dDeltaTime)
{
	CMesh::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::paint(CRenderContext* pContext)
{
	double dScale = pContext->camera()->getMaxDistance() * 0.75;

	setOriginScale(CVector3(dScale, dScale, dScale));
	setWorldTransform(pContext->camera()->getWorldTransform());

	CMesh::paint(pContext);
}

//-------------------------------------------------------------------------------------------------

void CSkyBox::loadParameters(CXMLNode xComponent)
{
	CMesh::loadParameters(xComponent);
}
