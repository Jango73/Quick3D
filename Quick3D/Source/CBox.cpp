
#include "CBox.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------
// Constantes

#define BOX_SIZE	0.5
#define COORD_STEP	((float) (1.0 / 4.0))

//-------------------------------------------------------------------------------------------------

CBox::CBox(C3DScene* pScene, double dMaxDistance)
: CMesh(pScene, dMaxDistance)
, m_vMinimum(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE)
, m_vMaximum( BOX_SIZE,  BOX_SIZE,  BOX_SIZE)
{
	setName("Box");

	m_bAllQuads = true;

	getMaterials()[0]->setIRFactor(0.4);

	m_vFaces.append(CFace(this,  0,  1,  2,  3));
	m_vFaces.append(CFace(this,  4,  5,  6,  7));
	m_vFaces.append(CFace(this,  8,  9, 10, 11));
	m_vFaces.append(CFace(this, 12, 13, 14, 15));
	m_vFaces.append(CFace(this, 16, 17, 18, 19));
	m_vFaces.append(CFace(this, 20, 21, 22, 23));

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
	m_vVertices.clear();

	// Front
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));

	// Top
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));

	// Right
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 3.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 3.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));

	// Left
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 0.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 0.0, COORD_STEP * 1.0)));

	// Back
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 4.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 4.0)));

	// Bottom
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
	m_vVertices.append(CVertex(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 0.0)));
	m_vVertices.append(CVertex(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 0.0)));

	updateGeometry();
}
