
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"

class C3DScene;

class QUICK3D_EXPORT CEngine : public CPhysicalComponent
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CEngine(C3DScene* pScene);

	//!
	virtual ~CEngine();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setMaxThrust_kg(double value);

	//!
	virtual void setCurrentFuelFlow_norm(double value);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	virtual double getCurrentFuelFlow_norm() const;

	//!
	virtual double getCurrentThrust_kg() const;

	//!
	virtual double getCurrentFuelCons_ls() const;

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CEngine; }

	//!
	virtual void loadParameters(CXMLNode xComponent);

	//!
	virtual void update(double dDeltaTime);

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	double m_dMaxThrust_kg;
	double m_dFuelFlow_norm;
};
