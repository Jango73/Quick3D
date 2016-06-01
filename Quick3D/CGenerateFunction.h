
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

#include "CPerlin.h"

class QUICK3D_EXPORT CGenerateFunction
{
public:

	enum ETerrainOperation
	{
		toNone,
		toConstant,
		toAdd,
		toSub,
		toMul,
		toDiv,
		toPow,
		toPerlin,
		toTurbulence,
		toErosion,
		toVoronoi
	};

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CGenerateFunction(CXMLNode xNode);

	//!
	virtual ~CGenerateFunction();

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void getProceduralParameters(CXMLNode xParams);

	//!
	double process(CPerlin* pPerlin, const Math::CVector3& vPosition, const Math::CAxis& aAxis) const;

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	ETerrainOperation			m_eType;
	double						m_dConstant;
	QVector<CGenerateFunction*>	m_vOperands;
	double						m_dInputScale;
	double						m_dOutputScale;
	double						m_dMinClamp;
	double						m_dMaxClamp;
	double						m_dDisplace;
	int							m_dIterations;
	double						m_dInputScaleFactor;
	double						m_dOutputScaleFactor;
};
